/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <trace/events/sched.h>
#include <trace/events/mtk_events.h>

#ifdef CONFIG_MTK_DRAMC
#include <mtk_dramc.h>
#endif

#include <linux/mm.h>
#include <linux/swap.h>
#include <mt-plat/mtk_blocktag.h>
#include <helio-dvfsrc.h>

#include <linux/module.h>
#ifndef __CHECKER__
#define CREATE_TRACE_POINTS
#include "perf_tracker_trace.h"
#endif

#ifdef CONFIG_MTK_QOS_FRAMEWORK
#include <mtk_qos_sram.h>
#endif

#ifdef CONFIG_MTK_PERF_OBSERVER
#include <mt-plat/mtk_perfobserver.h>
#endif

#include <mt-plat/perf_tracker.h>
#include <linux/arch_topology.h>
#include <perf_tracker_internal.h>

static int perf_tracker_on, perf_tracker_init;
static DEFINE_MUTEX(perf_ctl_mutex);
static int cluster_nr = -1;

#ifdef CONFIG_MTK_GPU_SWPM_SUPPORT
unsigned int gpu_pmu_enable;
unsigned int is_gpu_pmu_worked;
unsigned int gpu_pmu_period = 8000000; //8ms

void (*MTKGPUPower_model_start_symbol)(unsigned int interval_ns);
void (*MTKGPUPower_model_stop_symbol)(void);
void (*MTKGPUPower_model_suspend_symbol)(void);
void (*MTKGPUPower_model_resume_symbol)(void);
#endif

#if !defined(CONFIG_MTK_BLOCK_TAG) || !defined(MTK_BTAG_FEATURE_MICTX_IOSTAT)
struct mtk_btag_mictx_iostat_struct {
	__u64 duration;  /* duration time for below performance data (ns) */
	__u32 tp_req_r;  /* throughput (per-request): read  (KB/s) */
	__u32 tp_req_w;  /* throughput (per-request): write (KB/s) */
	__u32 tp_all_r;  /* throughput (overlapped) : read  (KB/s) */
	__u32 tp_all_w;  /* throughput (overlapped) : write (KB/s) */
	__u32 reqsize_r; /* request size : read  (Bytes) */
	__u32 reqsize_w; /* request size : write (Bytes) */
	__u32 reqcnt_r;  /* request count: read */
	__u32 reqcnt_w;  /* request count: write */
	__u16 wl;        /* storage device workload (%) */
	__u16 q_depth;   /* storage cmdq queue depth */
};
#endif

#ifdef CONFIG_MTK_BLOCK_TAG
static struct mtk_btag_mictx_iostat_struct iostatptr;

void  __attribute__((weak)) mtk_btag_mictx_enable(int enable) {}

int __attribute__((weak)) mtk_btag_mictx_get_data(
	struct mtk_btag_mictx_iostat_struct *io)
{
	return -1;
}
#endif

static void init_function_symbols(void)
{
#define _FUNC_SYMBOL_GET(_func_name_) \
	do { \
		_func_name_##_symbol = (void *)symbol_get(_func_name_); \
		if (_func_name_##_symbol == NULL) { \
			pr_debug("Symbol : %s is not found!\n", #_func_name_); \
		} \
	} while (0)
#ifdef CONFIG_MTK_GPU_SWPM_SUPPORT
	_FUNC_SYMBOL_GET(MTKGPUPower_model_start);
	_FUNC_SYMBOL_GET(MTKGPUPower_model_stop);
	_FUNC_SYMBOL_GET(MTKGPUPower_model_suspend);
	_FUNC_SYMBOL_GET(MTKGPUPower_model_resume);
#endif
}

int perf_tracker_enable(int val)
{
	mutex_lock(&perf_ctl_mutex);

	val = (val > 0) ? 1 : 0;

	perf_tracker_on = val;
#ifdef CONFIG_MTK_BLOCK_TAG
	mtk_btag_mictx_enable(val);
#endif

#ifdef CONFIG_MTK_GPU_SWPM_SUPPORT
	// GPU PMU Recording
	if (val == 1 && gpu_pmu_enable && !is_gpu_pmu_worked) {
		if (MTKGPUPower_model_start_symbol)
			MTKGPUPower_model_start_symbol(gpu_pmu_period);
			is_gpu_pmu_worked = 1;
	} else if (val == 0 && is_gpu_pmu_worked) {
		if (MTKGPUPower_model_stop_symbol)
			MTKGPUPower_model_stop_symbol();
		is_gpu_pmu_worked = 0;
	}
#endif

	mutex_unlock(&perf_ctl_mutex);

	return (perf_tracker_on == val) ? 0 : -1;
}

unsigned int __attribute__((weak)) get_dram_data_rate(void)
{
	return 0;
}

u32 __attribute__((weak)) qos_sram_read(u32 offset)
{
	return 0;
}

static inline u32 cpu_stall_ratio(int cpu)
{
#ifdef CM_STALL_RATIO_OFFSET
	return qos_sram_read(CM_STALL_RATIO_OFFSET + cpu * 4);
#else
	return 0;
#endif
}

#define K(x) ((x) << (PAGE_SHIFT - 10))
#define max_cpus 8

void __perf_tracker(u64 wallclock,
		    long mm_available,
		    long mm_free)
{
	int dram_rate = 0;
#ifdef CONFIG_MTK_BLOCK_TAG
	struct mtk_btag_mictx_iostat_struct *iostat = &iostatptr;
#endif
	int bw_c = 0, bw_g = 0, bw_mm = 0, bw_total = 0;
	int i;
	int stall[max_cpus] = {0};
	unsigned int sched_freq[3] = {0};
	int cid;

#ifdef CONFIG_MTK_PERF_OBSERVER
	pob_qos_tracker(wallclock);
#endif

	if (!perf_tracker_on || !perf_tracker_init)
		return;

	/* dram freq */
	dram_rate = get_dram_data_rate();

#ifdef CONFIG_MTK_QOS_FRAMEWORK
	/* emi */
	bw_c  = qos_sram_read(QOS_DEBUG_1);
	bw_g  = qos_sram_read(QOS_DEBUG_2);
	bw_mm = qos_sram_read(QOS_DEBUG_3);
	bw_total = qos_sram_read(QOS_DEBUG_0);
#endif
	/* sched: cpu freq */
	for (cid = 0; cid < cluster_nr; cid++)
		sched_freq[cid] = mt_cpufreq_get_cur_freq(cid);

	/* trace for short msg */
	trace_perf_index_s(
			sched_freq[0], sched_freq[1], sched_freq[2],
			dram_rate, bw_c, bw_g, bw_mm, bw_total
			);

	if (!hit_long_check())
		return;

	/* free mem */
	if (mm_free == -1) {
		mm_free = global_zone_page_state(NR_FREE_PAGES);
		mm_available = si_mem_available();
	}

#ifdef CONFIG_MTK_BLOCK_TAG
	/* IO stat */
	if (mtk_btag_mictx_get_data(iostat))
		memset(iostat, 0, sizeof(struct mtk_btag_mictx_iostat_struct));
#endif

	/* cpu stall ratio */
	for (i = 0; i < nr_cpu_ids || i < max_cpus; i++)
		stall[i] = cpu_stall_ratio(i);

	/* trace for long msg */
	trace_perf_index_l(
			K(mm_free),
			K(mm_available),
#ifdef CONFIG_MTK_BLOCK_TAG
			iostat->wl,
			iostat->tp_req_r, iostat->tp_all_r,
			iostat->reqsize_r, iostat->reqcnt_r,
			iostat->tp_req_w, iostat->tp_all_w,
			iostat->reqsize_w, iostat->reqcnt_w,
			iostat->duration, iostat->q_depth,
#else
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
			stall);
}

#ifdef CONFIG_MTK_GPU_SWPM_SUPPORT
void perf_update_gpu_counter(unsigned int gpu_data[], unsigned int len)
{
	trace_perf_index_gpu(gpu_data, len);
}
EXPORT_SYMBOL(perf_update_gpu_counter);

static ssize_t show_gpu_pmu_enable(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	unsigned int len = 0;
	unsigned int max_len = 4096;
	uint is_inited = 0;

	if (MTKGPUPower_model_start_symbol)
		is_inited = 1;

	len += snprintf(buf, max_len, "gpu_pmu_enable = %u, is_inited = %u\n",
			gpu_pmu_enable, is_inited);
	return len;
}

static ssize_t store_gpu_pmu_enable(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	mutex_lock(&perf_ctl_mutex);

	if (kstrtouint(buf, 10, &gpu_pmu_enable) == 0)
		gpu_pmu_enable = (gpu_pmu_enable > 0) ? 1 : 0;

	mutex_unlock(&perf_ctl_mutex);

	return count;
}


static ssize_t show_gpu_pmu_period(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	unsigned int len = 0;
	unsigned int max_len = 4096;

	len += snprintf(buf, max_len, "gpu_pmu_period = %u\n",
			gpu_pmu_period);
	return len;
}

static ssize_t store_gpu_pmu_period(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	mutex_lock(&perf_ctl_mutex);

	if (kstrtouint(buf, 10, &gpu_pmu_period) == 0) {
		if (gpu_pmu_period < 1000000) // 1ms
			gpu_pmu_period = 1000000;
	}

	mutex_unlock(&perf_ctl_mutex);

	return count;
}
#endif

/*
 * make perf tracker on
 * /sys/devices/system/cpu/perf/enable
 * 1: on
 * 0: off
 */
static ssize_t show_perf_enable(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	unsigned int len = 0;
	unsigned int max_len = 4096;

	len += snprintf(buf, max_len, "enable = %d\n",
			perf_tracker_on);
	return len;
}

static ssize_t store_perf_enable(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val = 0;

	if (sscanf(buf, "%iu", &val) != 0)
		perf_tracker_enable(val);

	return count;
}


static struct kobj_attribute perf_enable_attr =
__ATTR(enable, 0600, show_perf_enable, store_perf_enable);
#ifdef CONFIG_MTK_GPU_SWPM_SUPPORT
static struct kobj_attribute perf_gpu_pmu_enable_attr =
__ATTR(gpu_pmu_enable, 0600, show_gpu_pmu_enable, store_gpu_pmu_enable);
static struct kobj_attribute perf_gpu_pmu_period_attr =
__ATTR(gpu_pmu_period, 0600, show_gpu_pmu_period, store_gpu_pmu_period);
#endif

static struct attribute *perf_attrs[] = {
	&perf_enable_attr.attr,
#ifdef CONFIG_MTK_GPU_SWPM_SUPPORT
	&perf_gpu_pmu_enable_attr.attr,
	&perf_gpu_pmu_period_attr.attr,
#endif
	NULL,
};

static struct attribute_group perf_attr_group = {
	.attrs = perf_attrs,
};

static int init_perf_tracker(void)
{
	int ret = 0;
	struct kobject *kobj = NULL;

	perf_tracker_init = 1;
	cluster_nr = arch_get_nr_clusters();

	if (unlikely(cluster_nr <= 0 || cluster_nr > 3))
		cluster_nr = 3;

	kobj = kobject_create_and_add("perf", &cpu_subsys.dev_root->kobj);

	if (kobj) {
		ret = sysfs_create_group(kobj, &perf_attr_group);
		if (ret)
			kobject_put(kobj);
		else
			kobject_uevent(kobj, KOBJ_ADD);
	}

	init_function_symbols();

	return 0;
}
late_initcall_sync(init_perf_tracker);
