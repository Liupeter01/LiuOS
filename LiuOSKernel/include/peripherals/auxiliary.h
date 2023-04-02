/**
 * \ingroup peripherals
 * @file aux.h
 * @brief Definition of AUX registers addresses (Only for Uart).
 * @details AUX : Auxiliary Peripherals
 */
#ifndef P_AUX_H
#define P_AUX_H
#include <armv8-a/memlayout.h>

/**
 * \ingroup peripherals
 * AUX registers addresses.
 * 	The offsets for reach register for the UART 
	AUX_ENABLES     = (AUX_BASE + 0x04), *< Auxiliary Enables 
	AUX_MU_IO_REG   = (AUX_BASE + 0x40), *< Mini UART I/O Data 
	AUX_MU_IER_REG  = (AUX_BASE + 0x44), *< Mini UART Interrupt Enable 
	AUX_MU_IIR_REG  = (AUX_BASE + 0x48), *< Mini UART Interrupt Identify 
	AUX_MU_LCR_REG  = (AUX_BASE + 0x4C), *< Mini UART Line Control 
	AUX_MU_MCR_REG  = (AUX_BASE + 0x50), *< Mini UART Model Control 
	AUX_MU_LSR_REG  = (AUX_BASE + 0x54), *< Mini UART Line Status 
	AUX_MU_MSR_REG  = (AUX_BASE + 0x58), *< Mini UART Modem Status 
	AUX_MU_SCRATCH  = (AUX_BASE + 0x5C), *< Mini UART Scratch 
	AUX_MU_CNTL_REG = (AUX_BASE + 0x60), *< Mini UART Extra Control 
	AUX_MU_STAT_REG = (AUX_BASE + 0x64), *< Mini UART Extra Status 
	AUX_MU_BAUD_REG = (AUX_BASE + 0x68)  *< Mini UART Baudrate 
 */
enum
{
	AUX_ENABLES     = (0x04), /**< Auxiliary Enables */
	AUX_MU_IO_REG   = (0x40), /**< Mini UART I/O Data */
	AUX_MU_IER_REG  = (0x44), /**< Mini UART Interrupt Enable */
	AUX_MU_IIR_REG  = (0x48), /**< Mini UART Interrupt Identify */
	AUX_MU_LCR_REG  = (0x4C), /**< Mini UART Line Control */
	AUX_MU_MCR_REG  = (0x50), /**< Mini UART Model Control */
	AUX_MU_LSR_REG  = (0x54), /**< Mini UART Line Status */
	AUX_MU_MSR_REG  = (0x58), /**< Mini UART Modem Status */
	AUX_MU_SCRATCH  = (0x5C), /**< Mini UART Scratch */
	AUX_MU_CNTL_REG = (0x60), /**< Mini UART Extra Control */
	AUX_MU_STAT_REG = (0x64), /**< Mini UART Extra Status */
	AUX_MU_BAUD_REG = (0x68)  /**< Mini UART Baudrate */
};
#endif