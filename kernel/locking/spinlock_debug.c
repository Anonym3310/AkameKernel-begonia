/*
 * Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 *
 * This file contains the spinlock/rwlock implementations for
 * DEBUG_SPINLOCK.
 */

#include <linux/spinlock.h>
#include <linux/nmi.h>
#include <linux/interrupt.h>
#include <linux/debug_locks.h>
#include <linux/delay.h>
#include <linux/export.h>
#include "sched.h"
#include "lockdep_internals.h"

#ifdef CONFIG_MTK_AEE_FEATURE
#include <mt-plat/aee.h>
#endif

#ifdef CONFIG_MTK_SCHED_MONITOR
#include "mtk_sched_mon.h"
#endif

#ifdef MTK_LOCK_DEBUG
#include <linux/sched/clock.h>
#include <linux/sched/debug.h>

static long long msec_high(unsigned long long nsec)
{
	if ((long long)nsec < 0) {
		nsec = -nsec;
		do_div(nsec, 1000000);
		return -nsec;
	}
	do_div(nsec, 1000000);

	return nsec;
}

static long long sec_high(unsigned long long nsec)
{
	if ((long long)nsec < 0) {
		nsec = -nsec;
		do_div(nsec, 1000000000);
		return -nsec;
	}
	do_div(nsec, 1000000000);

	return nsec;
}

static unsigned long sec_low(unsigned long long nsec)
{
	if ((long long)nsec < 0)
		nsec = -nsec;
	/* exclude part of nsec */
	return do_div(nsec, 1000000000)/1000;
}
#endif

bool is_logbuf_lock_held(raw_spinlock_t *lock)
{
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	/* The lock is needed by kmalloc and aee_kernel_warning_api */
	if (!strcmp(lock->dep_map.name, "logbuf_lock"))
		return true;
#endif
	return false;
}

void __raw_spin_lock_init(raw_spinlock_t *lock, const char *name,
			  struct lock_class_key *key)
{
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	/*
	 * Make sure we are not reinitializing a held lock:
	 */
	debug_check_no_locks_freed((void *)lock, sizeof(*lock));
	lockdep_init_map(&lock->dep_map, name, key, 0);
#endif
	lock->raw_lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;
	lock->magic = SPINLOCK_MAGIC;
	lock->owner = SPINLOCK_OWNER_INIT;
	lock->owner_cpu = -1;
}

EXPORT_SYMBOL(__raw_spin_lock_init);

void __rwlock_init(rwlock_t *lock, const char *name,
		   struct lock_class_key *key)
{
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	/*
	 * Make sure we are not reinitializing a held lock:
	 */
	debug_check_no_locks_freed((void *)lock, sizeof(*lock));
	lockdep_init_map(&lock->dep_map, name, key, 0);
#endif
	lock->raw_lock = (arch_rwlock_t) __ARCH_RW_LOCK_UNLOCKED;
	lock->magic = RWLOCK_MAGIC;
	lock->owner = SPINLOCK_OWNER_INIT;
	lock->owner_cpu = -1;
}

EXPORT_SYMBOL(__rwlock_init);

static void spin_dump(raw_spinlock_t *lock, const char *msg)
{
	struct task_struct *owner = READ_ONCE(lock->owner);

	if (owner == SPINLOCK_OWNER_INIT)
		owner = NULL;
	printk(KERN_EMERG "BUG: spinlock %s on CPU#%d, %s/%d\n",
		msg, raw_smp_processor_id(),
		current->comm, task_pid_nr(current));
	printk(KERN_EMERG " lock: %pS, .magic: %08x, .owner: %s/%d, "
			".owner_cpu: %d\n",
		lock, READ_ONCE(lock->magic),
		owner ? owner->comm : "<none>",
		owner ? task_pid_nr(owner) : -1,
		READ_ONCE(lock->owner_cpu));
	dump_stack();
}

static void spin_bug(raw_spinlock_t *lock, const char *msg)
{
	char aee_str[50];

	if (!debug_locks_off())
		return;

	spin_dump(lock, msg);
	snprintf(aee_str, sizeof(aee_str), "%s: [%s]\n", current->comm, msg);

	if (!strcmp(msg, "bad magic") || !strcmp(msg, "already unlocked")
		|| !strcmp(msg, "wrong owner") || !strcmp(msg, "wrong CPU")
		|| !strcmp(msg, "uninitialized")) {
		pr_info("%s", aee_str);
		pr_info("maybe use an un-initial spin_lock or mem corrupt\n");
		pr_info("maybe already unlocked or wrong owner or wrong CPU\n");
		pr_info("maybe bad magic %08x, should be %08x\n",
			lock->magic, SPINLOCK_MAGIC);
		pr_info(">>>>>>>>>>>>>> Let's KE <<<<<<<<<<<<<<\n");
		BUG_ON(1);
	}

#ifdef CONFIG_MTK_AEE_FEATURE
	if (!is_critical_lock_held()) {
		aee_kernel_warning_api(__FILE__, __LINE__,
			DB_OPT_DUMMY_DUMP | DB_OPT_FTRACE,
			aee_str, "spinlock debugger\n");
	}
#endif
}

#define SPIN_BUG_ON(cond, lock, msg) if (unlikely(cond)) spin_bug(lock, msg)

static inline void
debug_spin_lock_before(raw_spinlock_t *lock)
{
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	SPIN_BUG_ON(lock->dep_map.name == NULL, lock, "uninitialized");
#endif
	SPIN_BUG_ON(READ_ONCE(lock->magic) != SPINLOCK_MAGIC, lock, "bad magic");
	SPIN_BUG_ON(READ_ONCE(lock->owner) == current, lock, "recursion");
	SPIN_BUG_ON(READ_ONCE(lock->owner_cpu) == raw_smp_processor_id(),
							lock, "cpu recursion");
}

static inline void debug_spin_lock_after(raw_spinlock_t *lock)
{
	WRITE_ONCE(lock->owner_cpu, raw_smp_processor_id());
	WRITE_ONCE(lock->owner, current);
}

static inline void debug_spin_unlock(raw_spinlock_t *lock)
{
	SPIN_BUG_ON(lock->magic != SPINLOCK_MAGIC, lock, "bad magic");
	SPIN_BUG_ON(!raw_spin_is_locked(lock), lock, "already unlocked");
	SPIN_BUG_ON(lock->owner != current, lock, "wrong owner");
	SPIN_BUG_ON(lock->owner_cpu != raw_smp_processor_id(),
							lock, "wrong CPU");
	WRITE_ONCE(lock->owner, SPINLOCK_OWNER_INIT);
	WRITE_ONCE(lock->owner_cpu, -1);
}

#ifdef MTK_LOCK_DEBUG
static void show_cpu_backtrace(void *ignored)
{
	pr_info("========== The call trace of lock owner on CPU%d ==========\n",
		raw_smp_processor_id());
	dump_stack();

	#ifdef CONFIG_MTK_AEE_FEATURE
	if (!is_critical_lock_held()) {
		char aee_str[64];

		snprintf(aee_str, sizeof(aee_str),
			"Spinlock lockup: (%s) hold spinlock over 1 second\n",
			current->comm);
		aee_kernel_warning_api(__FILE__, __LINE__,
			DB_OPT_DUMMY_DUMP | DB_OPT_FTRACE,
			aee_str, "spinlock debugger\n");
	}
	#endif
}
static DEFINE_PER_CPU(call_single_data_t, spinlock_debug_csd) = {
	.func = show_cpu_backtrace,
	.info = NULL,
	.flags = 0
};
#endif

/*Select appropriate loop counts to 1~2sec*/
#if HZ == 100
#define LOOP_HZ 100 /* temp 10 */
#elif HZ == 10
#define LOOP_HZ 2 /* temp 2 */
#else
#define LOOP_HZ HZ
#endif
#define WARNING_TIME 1000000000		/* warning time 1 seconds */

#ifdef MTK_LOCK_DEBUG
static void __spin_lock_debug(raw_spinlock_t *lock)
{
	u64 one_second = loops_per_jiffy * LOOP_HZ;
	u64 loops = one_second;
	u32 cpu = raw_smp_processor_id();
	int print_once = 1, cnt = 0;
	char lock_name[64];
	unsigned long long t1, t2, t3;
	struct task_struct *owner = NULL;
	cycles_t start = get_cycles();

	/* skip debugging */
	if (is_logbuf_lock_held(lock)) {
		arch_spin_lock(&lock->raw_lock);
		return;
	}

	t1 = sched_clock();
	t2 = t1;

	for (;;) {
		while ((get_cycles() - start) < loops) {
			if (arch_spin_trylock(&lock->raw_lock))
				return;
		}
		loops += one_second;

		t3 = sched_clock();
		if (t3 < t2)
			continue;
		if (t3 - t2 < WARNING_TIME)
			continue;
		t2 = sched_clock();

		/* lockup suspected: */
		if (lock->owner && lock->owner != SPINLOCK_OWNER_INIT)
			owner = lock->owner;

#ifdef CONFIG_DEBUG_LOCK_ALLOC
		snprintf(lock_name, 64, "%s", lock->dep_map.name);
#else
		snprintf(lock_name, 64, "%ps", lock);
#endif
		pr_info("(%s)(%p) spin time: %llu ms(from %lld.%06lu), raw_lock: 0x%08x, magic: %08x, held by %s/%d on CPU#%d(from %lld.%06lu)\n",
		lock_name, lock,
		msec_high(t2 - t1), sec_high(t1), sec_low(t1),
		*((unsigned int *)&lock->raw_lock), lock->magic,
		owner ? owner->comm : "<none>",
		owner ? task_pid_nr(owner) : -1, lock->owner_cpu,
		sec_high(lock->lock_t), sec_low(lock->lock_t));

		if (owner && (print_once || cnt == 5)) {
			debug_show_held_locks(owner);
			cnt = 0;
		}
		cnt++;

		if (oops_in_progress != 0)
			/* in exception follow, printk maybe spinlock error */
			continue;

		if (print_once) {
			print_once = 0;
			pr_info("========== The call trace of spinning task ==========\n");
			dump_stack();

			if (owner && (cpu != lock->owner_cpu))
				smp_call_function_single_async(
					lock->owner_cpu,
					this_cpu_ptr(&spinlock_debug_csd));
		}
	}
}
#endif /* MTK_LOCK_DEBUG */

/*
 * We are now relying on the NMI watchdog to detect lockup instead of doing
 * the detection here with an unfair lock which can cause problem of its own.
 */
void do_raw_spin_lock(raw_spinlock_t *lock)
{
#ifdef CONFIG_MTK_SCHED_MONITOR
	mt_trace_lock_spinning_start(lock);
#endif
	debug_spin_lock_before(lock);
#ifdef MTK_LOCK_DEBUG
	if (unlikely(!arch_spin_trylock(&lock->raw_lock)))
		__spin_lock_debug(lock);
#else
	arch_spin_lock(&lock->raw_lock);
#endif
	lock->lock_t = sched_clock();
	debug_spin_lock_after(lock);
#ifdef CONFIG_MTK_SCHED_MONITOR
	mt_trace_lock_spinning_end(lock);
#endif
}

int do_raw_spin_trylock(raw_spinlock_t *lock)
{
	int ret = arch_spin_trylock(&lock->raw_lock);

	if (ret)
		debug_spin_lock_after(lock);
#ifndef CONFIG_SMP
	/*
	 * Must not happen on UP:
	 */
	SPIN_BUG_ON(!ret, lock, "trylock failure on UP");
#endif
	return ret;
}

void do_raw_spin_unlock(raw_spinlock_t *lock)
{
	debug_spin_unlock(lock);
	arch_spin_unlock(&lock->raw_lock);
}

static void rwlock_bug(rwlock_t *lock, const char *msg)
{
	if (!debug_locks_off())
		return;

	printk(KERN_EMERG "BUG: rwlock %s on CPU#%d, %s/%d, %p\n",
		msg, raw_smp_processor_id(), current->comm,
		task_pid_nr(current), lock);
	dump_stack();
}

#define RWLOCK_BUG_ON(cond, lock, msg) if (unlikely(cond)) rwlock_bug(lock, msg)

void do_raw_read_lock(rwlock_t *lock)
{
	RWLOCK_BUG_ON(lock->magic != RWLOCK_MAGIC, lock, "bad magic");
	arch_read_lock(&lock->raw_lock);
}

int do_raw_read_trylock(rwlock_t *lock)
{
	int ret = arch_read_trylock(&lock->raw_lock);

#ifndef CONFIG_SMP
	/*
	 * Must not happen on UP:
	 */
	RWLOCK_BUG_ON(!ret, lock, "trylock failure on UP");
#endif
	return ret;
}

void do_raw_read_unlock(rwlock_t *lock)
{
	RWLOCK_BUG_ON(lock->magic != RWLOCK_MAGIC, lock, "bad magic");
	arch_read_unlock(&lock->raw_lock);
}

static inline void debug_write_lock_before(rwlock_t *lock)
{
	RWLOCK_BUG_ON(lock->magic != RWLOCK_MAGIC, lock, "bad magic");
	RWLOCK_BUG_ON(lock->owner == current, lock, "recursion");
	RWLOCK_BUG_ON(lock->owner_cpu == raw_smp_processor_id(),
							lock, "cpu recursion");
}

static inline void debug_write_lock_after(rwlock_t *lock)
{
	WRITE_ONCE(lock->owner_cpu, raw_smp_processor_id());
	WRITE_ONCE(lock->owner, current);
}

static inline void debug_write_unlock(rwlock_t *lock)
{
	RWLOCK_BUG_ON(lock->magic != RWLOCK_MAGIC, lock, "bad magic");
	RWLOCK_BUG_ON(lock->owner != current, lock, "wrong owner");
	RWLOCK_BUG_ON(lock->owner_cpu != raw_smp_processor_id(),
							lock, "wrong CPU");
	WRITE_ONCE(lock->owner, SPINLOCK_OWNER_INIT);
	WRITE_ONCE(lock->owner_cpu, -1);
}

void do_raw_write_lock(rwlock_t *lock)
{
	debug_write_lock_before(lock);
	arch_write_lock(&lock->raw_lock);
	debug_write_lock_after(lock);
}

int do_raw_write_trylock(rwlock_t *lock)
{
	int ret = arch_write_trylock(&lock->raw_lock);

	if (ret)
		debug_write_lock_after(lock);
#ifndef CONFIG_SMP
	/*
	 * Must not happen on UP:
	 */
	RWLOCK_BUG_ON(!ret, lock, "trylock failure on UP");
#endif
	return ret;
}

void do_raw_write_unlock(rwlock_t *lock)
{
	debug_write_unlock(lock);
	arch_write_unlock(&lock->raw_lock);
}
