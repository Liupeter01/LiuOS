/**
 * \ingroup lib
 * \defgroup printk printk
 * Basic print function.
 * @{
 * @file printk.h
 * @brief Definition of printk.
 * @details Prints a kernel message to the serial console.
 */
#ifndef PRINTK_H
#define PRINTK_H

#include <stdarg.h>
#include <driver/uart.h>
#include <common/string.h>
#include <common/stdlib.h>

/**
 * Prints a kernel message, like printf.
 * @param fmt format string
 * @return
 * - On success: 0
 * @details The formats supported by this implementation are: `d, c, s, x`.
 * To be supported: `u, l, ll`.
 * @see uart_putc(), itoa()
 */
int printk(const char *fmt, ...);

/**
 * Prints startup message
 * @see printk()
 */
void setupLogo();
/** @} */

#endif