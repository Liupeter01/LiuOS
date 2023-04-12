/**
 * \ingroup printk
 * @file printk.c
 * @brief Implementation of printk
 * @details Prints a kernel message to the serial console.
 */
#include <kernel/printk.h>

int printk(const char *fmt, ...)
{
	va_list ap;

	/* Determine required size */
	va_start(ap, fmt);

	/* Iterate over format string */
	while (*fmt != '\0') {
		/* If a conversion specification is found */
		if (*fmt == '%') {
			/* Find the conversion specifier */
			switch (*(++fmt)) {
			case '%':
				uart_putc('%');
				break;
			case 'x':
				/*
				 * Pull of the argument and
				 * Convert int to char* (string)
				 * Print string
				 */
				uart_puts(itoa(va_arg(ap, unsigned int)));
				break;
			case 'd':
				uart_puts(itoa(va_arg(ap, int)));
				break;
			case 'c':
				uart_putc(va_arg(ap, int));
				break;
			case 's':
				uart_puts(va_arg(ap, const char *));
				break;
			default:
				break;
			}
		}
		/* If new line, add carriage return */
		else if (*fmt == '\n') {
			uart_putc('\r');
			uart_putc('\n');
		}
		/* Else print the other characters */
		else {
			uart_putc(*fmt);
		}
		fmt++;
	}

	/* clean memory reserved for ap */
	va_end(ap);

	return 0;
}

/**
 * Prints startup message
 * @see printk()
 */
void setupLogo()
{
	printk(" \n\n_     _        ___  ____  \n");
	printk("| |   (_)_   _ / _ \\/ ___| \n");
	printk("| |   | | | | | | | \\___ \\ \n");
	printk("| |___| | |_| | |_| |___) |\n");
	printk("|_____|_|\\__,_|\\___/|____/ \n");                
	printk("\nLiuOS Starting...\n");
}