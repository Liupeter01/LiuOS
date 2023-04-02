/*os_hardwaredef.h*/
#ifndef _OS_HARDWAREDEF_H_
#define _OS_HARDWAREDEF_H_
#pragma once
#ifdef __aarch64__
#include"boottype.h"

/*
 * ARMv8 ARM reserves the following encoding for system registers:
 * (Ref: ARMv8 ARM, Section: "System instruction class encoding overview",
 *  C5.2, version:ARM DDI 0487A.f)
 *	[20-19] : Op0
 *	[18-16] : Op1
 *	[15-12] : CRn
 *	[11-8]  : CRm
 *	[7-5]   : Op2
 */
#define Op0_shift	19
#define Op0_mask	0x3
#define Op1_shift	16
#define Op1_mask	0x7
#define CRn_shift	12
#define CRn_mask	0xf
#define CRm_shift	8
#define CRm_mask	0xf
#define Op2_shift	5
#define Op2_mask	0x7

#define sys_reg(op0, op1, crn, crm, op2) \
	(((op0) << Op0_shift) | ((op1) << Op1_shift) | \
	 ((crn) << CRn_shift) | ((crm) << CRm_shift) | \
	 ((op2) << Op2_shift))

/*-----------------------------------------------------
*****查询CPU支持的物理地址空间范围ID_AA64MMFR0_EL1********
-----------------------------------------------------*/
#define SYS_ID_MMFR0_EL1		sys_reg(3, 0, 0, 1, 4)	//访问系统支持的最大物理内存
#define SYS_ID_MMFR1_EL1		sys_reg(3, 0, 0, 1, 5)
#define SYS_ID_MMFR2_EL1		sys_reg(3, 0, 0, 1, 6)
#define SYS_ID_MMFR3_EL1		sys_reg(3, 0, 0, 1, 7)

#define ID_AA64MMFR0_PARANGE_48		0x5			//48位地址
#define ID_AA64MMFR0_PARANGE_52		0x6			//52位地址
#define ID_AA64MMFR0_TGRAN4_SHIFT  28			//判断是否支持4KB页面粒度
#define ID_AA64MMFR0_TGRAN4_SUPPORTED	0x0		//是否支持4KB页面粒度

/*-----------------------------------------------------
*****************转换控制寄存器TCR_EL?******************
-----------------------------------------------------*/
#define SYS_TCR_EL1			sys_reg(3, 0, 2, 0, 2)

#define TCR_IPS_SHIFT		32						  		//配置地址转换MMU后输出物理地址的最大值
#define TCR_IPS_MASK		(7UL << TCR_IPS_SHIFT)    		//TCR[34:32]

#define TCR_T0SZ_OFFSET		0						  		//TCR[5:0]
#define TCR_T1SZ_OFFSET		16								//TCR[21:16]
#define TCR_T0SZ(x)		((64UL - (x)) << TCR_T0SZ_OFFSET)	//低端虚拟地址区域0~FFFF000000000000
#define TCR_T1SZ(x)		((64UL - (x)) << TCR_T1SZ_OFFSET)	//高端虚拟地址区域FFFF000000000000~FFFFFFFFFFFFFFFF
#define TCR_TxSZ(x)		(TCR_T0SZ(x) | TCR_T1SZ(x))			//配置输入地址的最大值
#define TCR_TxSZ_WIDTH	6									//T0SZ和T1SZ的宽度
#define TCR_T0SZ_MASK	(((UL(1) << TCR_TxSZ_WIDTH) - 1) << TCR_T0SZ_OFFSET)

#define TCR_TG0_SHIFT	14									//用于设置TTBR0页表分页大小TCR[15:14]
#define TCR_TG0_MASK	(3 << TCR_TG0_SHIFT)				//
#define TCR_TG0_4K		(0 << TCR_TG0_SHIFT)				//4KB为0b00

#define TCR_TG1_SHIFT	30									//用于设置TTBR1页表分页大小TCR[31:30]
#define TCR_TG1_MASK	(3 << TCR_TG1_SHIFT)				//
#define TCR_TG1_4K		(2 << TCR_TG1_SHIFT)				//4KB为0b00

/*-----------------------------------------------------
*****************xxxxxxxxxxxxxxxxxxxx******************
-----------------------------------------------------*/
#define SYS_TTBR0_EL1			sys_reg(3, 0, 2, 0, 0)
#define SYS_TTBR1_EL1			sys_reg(3, 0, 2, 0, 1)

/*-----------------------------------------------------
*****************SCTLR_ELx*********************
-----------------------------------------------------*/
#define _BITUL(x)	(1UL << (x))

#define SCTLR_ELx_DSSBS	(_BITUL(44))
#define SCTLR_ELx_ENIA	(_BITUL(31))
#define SCTLR_ELx_ENIB	(_BITUL(30))
#define SCTLR_ELx_ENDA	(_BITUL(27))
#define SCTLR_ELx_EE    (_BITUL(25))
#define SCTLR_ELx_IESB	(_BITUL(21))
#define SCTLR_ELx_WXN	(_BITUL(19))
#define SCTLR_ELx_ENDB	(_BITUL(13))
#define SCTLR_ELx_I	(_BITUL(12))
#define SCTLR_ELx_SA	(_BITUL(3))
#define SCTLR_ELx_C	(_BITUL(2))
#define SCTLR_ELx_A	(_BITUL(1))
#define SCTLR_ELx_M	(_BITUL(0))				//用于打开MMU

#define MT_DEVICE_nGnRnE	0   //设备内存，不支持聚合、指令重排以及提前写应答
#define MT_DEVICE_nGnRE		1   //设备内存，不支持聚合、指令重排，但是支持提前写应答
#define MT_DEVICE_GRE		2   //设备内存，支持聚合、指令重排以及提前写应答
#define MT_NORMAL_NC		3   //普通内存，关闭高速缓存
#define MT_NORMAL		    4   //普通内存(内核的可执行代码段和数据段采用该部分内存!!!!!!!)
#define MT_NORMAL_WT		5   //普通内存，高速缓存采用直写回写策略

/*参考内存属性编码将内存属性信息写入MAIR_EL?
* Memory region attributes for LPAE:
*
*   n = AttrIndx[2:0]
*                  n   MAIR
*   DEVICE_nGnRnE  000 00000000 
*   DEVICE_nGnRE   001 00000100
*   DEVICE_GRE     010 00001100
*   NORMAL_NC      011 01000100
*   NORMAL         100 11111111
*   NORMAL_WT      101 10111011
-------------------------------------------------*/
#define MAIR(index,attribute) ((attribute) << ((index) * 8))   //每一次偏移一个字节

/*-----------------------------------------------------------------------------
* 初始化CPU
* @name: Enable_CPU
* @function:初始化CPU
*------------------------------------------------------------------------------*/
VOID 
EFIAPI 
Enable_CPU();

/*-----------------------------------------------------------------------------
* 启动MMU硬件
* @name: Enable_MMU
* @function: 启动MMU硬件并将地址写入硬件寄存器中
* @param :  1.恒等映射PGD(TTBR0_EL1)
            2.非恒等映射的内核基地址PGD(TTBR1_EL1)

* @retvalue: 返回MMU是否初始化成功
*------------------------------------------------------------------------------*/
Bool 
EFIAPI 
Enable_MMU(
	IN struct page_global_directory * idmap_pg_dir,
	IN struct page_global_directory * swapper_pg_dir
);


#endif //__aarch64__
#endif //_OS_HARDWAREDEF_H_