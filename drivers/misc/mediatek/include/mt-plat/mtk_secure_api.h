/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */


#ifndef _MTK_SECURE_API_H_
#define _MTK_SECURE_API_H_

#include <linux/kernel.h>


/* Error Code */
#define SIP_SVC_E_SUCCESS               0
#define SIP_SVC_E_NOT_SUPPORTED         -1
#define SIP_SVC_E_INVALID_PARAMS        -2
#define SIP_SVC_E_INVALID_Range         -3
#define SIP_SVC_E_PERMISSION_DENY       -4

#ifdef CONFIG_ARM64
#define MTK_SIP_SMC_AARCH_BIT			0x40000000
#else
#define MTK_SIP_SMC_AARCH_BIT			0x00000000
#endif

/*	0x82000200 -	0x820003FF &	0xC2000300 -	0xC20003FF */
/* Debug feature and ATF related */
#define MTK_SIP_KERNEL_WDT \
	(0x82000200 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_GIC_DUMP \
	(0x82000201 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_TIME_SYNC \
	(0x82000202 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AEE_DUMP \
	(0x82000203 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_ATF_DEBUG \
	(0x82000204 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_DFD \
	(0x82000205 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_CCCI_GET_INFO \
	(0x82000209 | MTK_SIP_SMC_AARCH_BIT)

/* CPU operations */
#define MTK_SIP_POWER_DOWN_CLUSTER \
	(0x82000210 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_POWER_UP_CLUSTER \
	(0x82000211 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_POWER_DOWN_CORE	\
	(0x82000212 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_POWER_UP_CORE \
	(0x82000213 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_POWER_FLOW_DEBUG \
	(0x82000214 | MTK_SIP_SMC_AARCH_BIT)

/* SPM related SMC call */
#define MTK_SIP_KERNEL_SPM_SUSPEND_ARGS \
	(0x82000220 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_SPM_FIRMWARE_STATUS \
	(0x82000221 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_SPM_IRQ0_HANDLER	\
	(0x82000222 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_SPM_AP_MDSRC_REQ	\
	(0x82000223 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_SPM_PWR_CTRL_ARGS \
	(0x82000224 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_SPM_LEGACY_SLEEP	\
	(0x82000225 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_SPM_VCOREFS_ARGS	\
	(0x82000226 | MTK_SIP_SMC_AARCH_BIT)

/* SPM deepidle related SMC call */
#define MTK_SIP_KERNEL_SPM_DPIDLE_ARGS \
	(0x82000227 | MTK_SIP_SMC_AARCH_BIT)

/* SPM SODI related SMC call */
#define MTK_SIP_KERNEL_SPM_SODI_ARGS \
	(0x82000228 | MTK_SIP_SMC_AARCH_BIT)

/* SPM sleep deepidle related SMC call */
#define MTK_SIP_KERNEL_SPM_SLEEP_DPIDLE_ARGS \
	(0x82000229 | MTK_SIP_SMC_AARCH_BIT)
/* SPM ARGS */
#define MTK_SIP_KERNEL_SPM_ARGS	\
	(0x8200022A | MTK_SIP_SMC_AARCH_BIT)

/* SPM get pwr_ctrl args */
#define MTK_SIP_KERNEL_SPM_GET_PWR_CTRL_ARGS \
	(0x8200022B | MTK_SIP_SMC_AARCH_BIT)

/* SPM Check security CG (for deepidle/SODI) */
#define MTK_SIP_KERNEL_CHECK_SECURE_CG \
	(0x8200022D | MTK_SIP_SMC_AARCH_BIT)

/* SPM LP related SMC call */
#define MTK_SIP_KERNEL_SPM_LP_ARGS \
	(0x8200022E | MTK_SIP_SMC_AARCH_BIT)

/* Low power Misc. 1 SMC call,
 * 0x82000230 -	0x8200023F &	0xC2000230 -	0xC200023F
 */
/* DCM SMC call */
#define MTK_SIP_KERNEL_DCM \
	(0x82000230 | MTK_SIP_SMC_AARCH_BIT)
/* MCDI related SMC call */
#define MTK_SIP_KERNEL_MCDI_ARGS \
	(0x82000231 | MTK_SIP_SMC_AARCH_BIT)
/* SCP DVFS related SMC call */
#define MTK_SIP_KERNEL_SCP_DVFS_CTRL \
	(0x82000232 | MTK_SIP_SMC_AARCH_BIT)


/* AMMS related SMC call */
#define MTK_SIP_KERNEL_AMMS_GET_FREE_ADDR \
	(0x82000250 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_GET_FREE_LENGTH \
	(0x82000251 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_GET_PENDING \
	(0x82000252 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_ACK_PENDING \
	(0x82000253 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_MD_POS_ADDR \
	(0x82000254 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_MD_POS_LENGTH \
	(0x82000255 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_GET_MD_POS_ADDR \
	(0x82000256 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_GET_MD_POS_LENGTH \
	(0x82000257 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_GET_SEQ_ID \
	(0x82000258 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_AMMS_POS_STRESS_TOUCH\
	(0x82000259 | MTK_SIP_SMC_AARCH_BIT)



/* Security related SMC call */
/* EMI MPU */
#define MTK_SIP_KERNEL_EMIMPU_WRITE (0x82000260 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_EMIMPU_READ  (0x82000261 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_EMIMPU_SET \
	(0x82000262 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_EMIMPU_CLEAR \
	(0x82000263 | MTK_SIP_SMC_AARCH_BIT)
/* DEVMPU */
#define MTK_SIP_KERNEL_DEVMPU_VIO_GET \
	(0x82000264 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_DEVMPU_PERM_GET \
	(0x82000265 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_DEVMPU_VIO_CLR \
	(0x82000268 | MTK_SIP_SMC_AARCH_BIT)

/* TRNG */
#define MTK_SIP_KERNEL_GET_RND \
	(0x8200026A | MTK_SIP_SMC_AARCH_BIT)
/* DEVAPC/SRAMROM */
#define MTK_SIP_KERNEL_DAPC_PERM_GET \
	(0x8200026B | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_CLR_SRAMROM_VIO \
	(0x8200026C | MTK_SIP_SMC_AARCH_BIT)


/* Storage Encryption related SMC call */
/* HW FDE related SMC call */
#define MTK_SIP_KERNEL_HW_FDE_UFS_CTL \
	(0x82000270 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_HW_FDE_AES_INIT \
	(0x82000271 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_HW_FDE_KEY   \
	(0x82000272 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_HW_FDE_MSDC_CTL \
	(0x82000273 | MTK_SIP_SMC_AARCH_BIT)
/* HIE related SMC call */
#define MTK_SIP_KERNEL_CRYPTO_HIE_CFG_REQUEST \
	(0x82000274 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_CRYPTO_HIE_INIT \
	(0x82000275 | MTK_SIP_SMC_AARCH_BIT)
/* UFS generic SMC call */
#define MTK_SIP_KERNEL_UFS_CTL \
	(0x82000276 | MTK_SIP_SMC_AARCH_BIT)

/* Platform related SMC call */
#define MTK_SIP_KERNEL_CACHE_FLUSH_FIQ \
	(0x82000280 | MTK_SIP_SMC_AARCH_BIT)
#define MTK_SIP_KERNEL_CACHE_FLUSH_INIT \
	(0x82000281 | MTK_SIP_SMC_AARCH_BIT)

#define MTK_SIP_KERNEL_CACHE_FLUSH_BY_SF \
	(0x82000283 | MTK_SIP_SMC_AARCH_BIT)

/* Pheripheral related SMC call */
#define MTK_SIP_KERNEL_I2C_SEC_WRITE \
	(0x820002A0 | MTK_SIP_SMC_AARCH_BIT)
/* M4U related SMC call */
#define MTK_M4U_DEBUG_DUMP \
	(0x820003E0 | MTK_SIP_SMC_AARCH_BIT)

/* APU related SMC call */
#define MTK_APU_VCORE_CG_CTL \
	(0x820003E9 | MTK_SIP_SMC_AARCH_BIT)

/* TEEs related SMC call */
#define MTK_SIP_KERNEL_TEE_CONTROL \
        (0x82000516 | MTK_SIP_SMC_AARCH_BIT)

/* cmdq related SMC call */
#define MTK_SIP_CMDQ_CONTROL \
        (0x82000518 | MTK_SIP_SMC_AARCH_BIT)

extern size_t mt_secure_call_all(size_t function_id,
	size_t arg0, size_t arg1, size_t arg2,
	size_t arg3, size_t *r1, size_t *r2, size_t *r3);

#define mt_secure_call(_fun_id, _arg0, _arg1, _arg2, _arg3) \
	mt_secure_call_all(_fun_id, _arg0, _arg1, _arg2, _arg3, 0, 0, 0)
#define mt_secure_call_ret1(_fun_id, _arg0, _arg1, _arg2, _arg3) \
	mt_secure_call_all(_fun_id, _arg0, _arg1, _arg2, _arg3, 0, 0, 0)
#define mt_secure_call_ret2(_fun_id, _arg0, _arg1, _arg2, _arg3, _r1) \
	mt_secure_call_all(_fun_id, _arg0, _arg1, _arg2, _arg3, _r1, 0, 0)
#define mt_secure_call_ret3(_fun_id, _arg0, _arg1, _arg2, _arg3, _r1, _r2) \
	mt_secure_call_all(_fun_id, _arg0, _arg1, _arg2, _arg3, _r1, _r2, 0)
#define mt_secure_call_ret4(_fun_id, _arg0,\
	_arg1, _arg2, _arg3, _r1, _r2, _r3) \
	mt_secure_call_all(_fun_id, _arg0, _arg1, _arg2, _arg3, _r1, _r2, _r3)

#endif				/* _MTK_SECURE_API_H_ */

#define emi_mpu_smc_write(offset, val) \
mt_secure_call(MTK_SIP_KERNEL_EMIMPU_WRITE, offset, val, 0, 0)

#define emi_mpu_smc_read(offset) \
mt_secure_call(MTK_SIP_KERNEL_EMIMPU_READ, offset, 0, 0, 0)

#ifdef CONFIG_ARM64
#define emi_mpu_smc_set(start, end, region_permission) \
mt_secure_call(MTK_SIP_KERNEL_EMIMPU_SET, start, end, region_permission, 0)
#else
#define emi_mpu_smc_set(start, end, apc8, apc0) \
mt_secure_call_all(MTK_SIP_KERNEL_EMIMPU_SET,\
start, end, apc8, apc0, 0, 0, 0, 0)
#endif

#define emi_mpu_smc_clear(region) \
mt_secure_call(MTK_SIP_KERNEL_EMIMPU_CLEAR, region, 0, 0, 0)

#define emi_mpu_smc_protect(start, end, apc) \
mt_secure_call(MTK_SIP_KERNEL_EMIMPU_SET, start, end, apc, 0)
