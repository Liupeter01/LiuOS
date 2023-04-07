/**
 * \ingroup drivers
 * @file gpio.h
 * @brief Definition of functions for manipulating GPIO registers.
 */

#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>
#include <armv8-a/aarch64.h>
#include <armv8-a/memlayout.h>

/**
 * \ingroup peripherals
 * GPIO Pins available functionalities: 5 Alternative Functions, Input, Output.
 */
typedef enum {
	GF_INPUT  = 0, /**< Input */
	GF_OUTPUT = 1, /**< Output */
	GF_ALT_0  = 4, /**< Alternative Function 0 */
	GF_ALT_1  = 5, /**< Alternative Function 1 */
	GF_ALT_2  = 6, /**< Alternative Function 2 */
	GF_ALT_3  = 7, /**< Alternative Function 3 */
	GF_ALT_4  = 3, /**< Alternative Function 4 */
	GF_ALT_5  = 2, /**< Alternative Function 5 */
} gpio_func;

/**
 * \ingroup peripherals
 * GPIO registers offset
 * @method: ((unsigned char*)GPIO_BASE+offset)
 * 
 * 	GPFSEL0 = (GPIO_BASE + 0x00), *< GPFSEL0: pins 0-9
	GPFSEL1 = (GPIO_BASE + 0x04), *< GPFSEL1: pins 10-19 
	GPFSEL2 = (GPIO_BASE + 0x08), *< GPFSEL2: pins 20-29 
	GPFSEL3 = (GPIO_BASE + 0x0C), *< GPFSEL3: pins 30-39 
	GPFSEL4 = (GPIO_BASE + 0x10), *< GPFSEL4: pins 40-49 
	GPFSEL5 = (GPIO_BASE + 0x14), *< GPFSEL5: pins 50-53 or 50-57 

 *	GPSET0 = (GPIO_BASE + 0x1C), *< The GPSET0 register is used to set GPIO pins 0-31 
	GPSET1 = (GPIO_BASE + 0x20), *< The GPSET1 register is used to set GPIO pins 32-53 

 *  GPCLR0 = (GPIO_BASE + 0x28), *< The GPCLR0 register is used to clear GPIO pins 0-31 
	GPCLR1 = (GPIO_BASE + 0x2C), *< The GPCLR1 register is used to clear GPIO pins 32-53 

 * 	GPLEV0 = (GPIO_BASE + 0x34), *< GPIO Pin Level 0 
	GPLEV1 = (GPIO_BASE + 0x38), *< GPIO Pin Level 1 
	GPEDS0 = (GPIO_BASE + 0x40), *< GPIO Pin Event Detect Status 0 
	GPEDS1 = (GPIO_BASE + 0x44), *< GPIO Pin Event Detect Status 1 
	GPREN0 = (GPIO_BASE + 0x4C), *< GPIO Pin Rising Edge Detect Enable 0 
	GPREN1 = (GPIO_BASE + 0x50), *< GPIO Pin Rising Edge Detect Enable 1 
	GPFEN0 = (GPIO_BASE + 0x58), *< GPIO Pin Falling Edge Detect Enable 0 
	GPFEN1 = (GPIO_BASE + 0x5C), *< GPIO Pin Falling Edge Detect Enable 1 
	GPHEN0 = (GPIO_BASE + 0x64), *< GPIO Pin High Detect Enable 0 
	GPHEN1 = (GPIO_BASE + 0x68), *< GPIO Pin High Detect Enable 1 
	GPLEN0 = (GPIO_BASE + 0x70),  *< GPIO Pin Low Detect Enable 0 
	GPLEN1 = (GPIO_BASE + 0x74),  *< GPIO Pin Low Detect Enable 1 
	GPAREN0 = (GPIO_BASE + 0x7C), *< GPIO Pin Async. Rising Edge Detect 0 
	GPAREN1 = (GPIO_BASE + 0x80), *< GPIO Pin Async. Rising Edge Detect 1 
	GPAFEN0 = (GPIO_BASE + 0x88), *< GPIO Pin Async. Falling Edge Detect 0 
	GPAFEN1 = (GPIO_BASE + 0x8C), *< GPIO Pin Async. Falling Edge Detect 1 

  	GPIO_PUP_PDN_CNTRL_REG0 = (GPIO_BASE + 0xE4), *< GPIO Pull-up / Pull-down Registers 0 
	GPIO_PUP_PDN_CNTRL_REG1 = (GPIO_BASE + 0xE8), *< GPIO Pull-up / Pull-down Registers 1 
	GPIO_PUP_PDN_CNTRL_REG2 = (GPIO_BASE + 0xEC), *< GPIO Pull-up / Pull-down Registers 2 
	GPIO_PUP_PDN_CNTRL_REG3 = (GPIO_BASE + 0xF0)  *< GPIO Pull-up / Pull-down Registers 3 
 */
enum
{
	GPFSEL0 = (0x00), /**< GPFSEL0: pins 0-9 */
	GPFSEL1 = (0x04), /**< GPFSEL1: pins 10-19 */
	GPFSEL2 = (0x08), /**< GPFSEL2: pins 20-29 */
	GPFSEL3 = (0x0C), /**< GPFSEL3: pins 30-39 */
	GPFSEL4 = (0x10), /**< GPFSEL4: pins 40-49 */
	GPFSEL5 = (0x14), /**< GPFSEL5: pins 50-53 or 50-57 */

	GPSET0 = (0x1C), /**< The GPSET0 register is used to set GPIO pins 0-31 */
	GPSET1 = (0x20), /**< The GPSET1 register is used to set GPIO pins 32-53 */
	
	GPCLR0 = (0x28), /**< The GPCLR0 register is used to clear GPIO pins 0-31 */
	GPCLR1 = (0x2C), /**< The GPCLR1 register is used to clear GPIO pins 32-53 */
	
	GPLEV0 = (0x34), /**< GPIO Pin Level 0 */
	GPLEV1 = (0x38), /**< GPIO Pin Level 1 */
	GPEDS0 = (0x40), /**< GPIO Pin Event Detect Status 0 */
	GPEDS1 = (0x44), /**< GPIO Pin Event Detect Status 1 */
	GPREN0 = (0x4C), /**< GPIO Pin Rising Edge Detect Enable 0 */
	GPREN1 = (0x50), /**< GPIO Pin Rising Edge Detect Enable 1 */
	GPFEN0 = (0x58), /**< GPIO Pin Falling Edge Detect Enable 0 */
	GPFEN1 = (0x5C), /**< GPIO Pin Falling Edge Detect Enable 1 */
	GPHEN0 = (0x64), /**< GPIO Pin High Detect Enable 0 */
	GPHEN1 = (0x68), /**< GPIO Pin High Detect Enable 1 */
	GPLEN0 = (0x70), /**< GPIO Pin Low Detect Enable 0 */
	GPLEN1 = (0x74), /**< GPIO Pin Low Detect Enable 1 */
	GPAREN0 = (0x7C), /**< GPIO Pin Async. Rising Edge Detect 0 */
	GPAREN1 = (0x80), /**< GPIO Pin Async. Rising Edge Detect 1 */
	GPAFEN0 = (0x88), /**< GPIO Pin Async. Falling Edge Detect 0 */
	GPAFEN1 = (0x8C), /**< GPIO Pin Async. Falling Edge Detect 1 */

	GPIO_PUP_PDN_CNTRL_REG0 = (0xE4), /**< GPIO Pull-up / Pull-down Registers 0 */
	GPIO_PUP_PDN_CNTRL_REG1 = (0xE8), /**< GPIO Pull-up / Pull-down Registers 1 */
	GPIO_PUP_PDN_CNTRL_REG2 = (0xEC), /**< GPIO Pull-up / Pull-down Registers 2 */
	GPIO_PUP_PDN_CNTRL_REG3 = (0xF0)  /**< GPIO Pull-up / Pull-down Registers 3 */
};

/**
 * \ingroup drivers
 * Sets the Alternative Function @a func for GPIO pin at @a pin_num, using GPFSELn
 * ( @ref GPFSEL0 - @ref GPFSEL5).
 * @param pin_num GPIO pin number.
 * @param func Alternative Function.
 * @return
 * - On success: 0
 * - On failure: -1
 * @see gpio_func, peripherals/gpio.h
 */
int gpio_pin_set_func(uint8_t pin_num, gpio_func func);

/**
 * \ingroup drivers
 * Sets the GPIO pin (High) at @a pin_num, using GPSETn ( @ref GPSET0, @ref GPSET1).
 * @param pin_num GPIO pin number.
 * @return
 * - On success: 0
 * - On failure: -1
 *
 * @see peripherals/gpio.h
 */
int gpio_pin_set(uint8_t pin_num);

/**
 * \ingroup drivers
 * Clears the GPIO pin (Low) at @a pin_num, using GPCLRn ( @ref GPCLR0, @ref GPCLR1).
 * @param pin_num GPIO pin number.
 * @return
 * - On success: 0
 * - On failure: -1
 *
 * @see peripherals/gpio.h
 */
int gpio_pin_clear(uint8_t pin_num);

#endif