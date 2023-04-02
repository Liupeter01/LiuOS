/**
 * \ingroup drivers
 * @file uart.c
 * @brief Implementation of basic UART functions
 */

#include <driver/uart.h>

/**
 * Uart Base Address, we consider about vitual mmio mapping problem
 */
volatile uchar *kernel_gpio_base = NULL;

/**
 * aux Base Address, we consider about vitual mmio mapping problem
 */
volatile uchar *kernel_aux_base = NULL;

/**
 * Initializes the UART interface based on the running device.
 * @param :use PA/VA address to init  base address
 * @details Basic configuration:
 * - Sets Alternative Function 5 for GPIO pins 14, 15, in order to enable mini UART.
 * - Disables pull up/down resistors for pins 14, 15.
 * - Enables mini UART.
 * - Disables auto flow control and disables receiver and transmitter.
 * - Enables receive and transmit interrupts.
 * - Clears the receive and transmit FIFO, and enables FIFO.
 * - Enables 8 bit mode.
 * - Sets RTS line to be always high.
 * - Sets baud rate at 115200.
 * - Enables transmitter and receiver.
 * @see peripherals/gpio.h, peripherals/aux.h
 */
void uart_init(uint64_t GPIO_addr)
{
	/*
	* init basic address
	*/
	kernel_gpio_base = (uchar*)GPIO_addr;					//init uart base address(PA or VA)
	kernel_aux_base = &kernel_gpio_base[AUX_BASE];				//locate kernel_aux_base
	/*
	 * Setup the GPIO pin 14 && 15
	 */
	/* Set Alternative Function 5 for GPIO pins 14, 15
	 * Enables mini UART for boards that use it as a primary UART
	 * Boards: Raspi Zero W, Raspi 3, Raspi 4
	 */
	uint32_t selector = mmio_read((uint64_t)&kernel_gpio_base[GPFSEL1]);
	selector &= ~(7 << 12);		/* Clear GPIO PIN 14 */
	selector |= 2 << 12;		/* Set Alt 5 for GPIO PIN 14 */
	selector &= ~(7 << 15);		/* Clear GPIO PIN 15 */
	selector |= 2 << 15;		/* Set Alt 5 for GPIO PIN 15 */
	mmio_write((uint64_t)&kernel_gpio_base[GPFSEL1], selector);

	/*
	 * Disable pull up/down for pin 14 && 15
	 */

	/* Disable pull up/down for pin 14 (TXD), 15 (RXD) */
	selector = mmio_read((uint64_t)&kernel_gpio_base[GPIO_PUP_PDN_CNTRL_REG0]);

	/* 31:30 bits - Resistor Select for GPIO15 */
	/* 29:28 bits - Resistor Select for GPIO14 */
	/* Clear GPIO15, GPIO14 Resistor Select pins -> Disables pull up/down */
	selector &= ~((1 << 31) | (1 << 30) | (1 << 29) | (1 << 28));
	mmio_write((uint64_t)&kernel_gpio_base[GPIO_PUP_PDN_CNTRL_REG0], selector);

	/* Enable mini UART*/
	mmio_write((uint64_t)&kernel_aux_base[AUX_ENABLES], 1);
	/* Disable auto flow control and disable receiver and transmitter */
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_CNTL_REG], 0);

	/* Disable receive and transmit interrupts */
	// mmio_write(AUX_MU_IER_REG, 0);

	/*
	 * Bit 0: Enable receive interrupt
	 * Bit 1: Enable transmit interrupt
	 * Bit 2&3: Required in order to receive interrupts
	 */
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_IER_REG], (1 << 0) | (1 << 2) | (1 << 3));

	/* Clear the receive and transmit FIFO, and enables FIFO */
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_IIR_REG], 0xC6);

	/* Enable 8 bit mode */
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_LCR_REG], 3);
	/* Set RTS line to be always high */
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_MCR_REG], 0);

	/* Set baud rate to 115200 */
	/* System_Clock_Freq = 500 MHz */
	/* (( System_Clock_Freq / baudrate_reg) / 8 ) - 1 */
	/* ((500,000,000 / 115200) / 8) - 1 = 541 */
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_BAUD_REG], 541);

	/* Finally, enable transmitter and receiver */
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_CNTL_REG], 3);
}

/**
 * Sends a byte to the UART (serial output).
 * @param c The byte sent to the UART.
 * @details First waits for the UART to become ready to transmit,
 * and then writes the byte @a c to the @ref AUX_MU_IO_REG register.
 * @see peripherals/aux.h
 */
void uart_putc(unsigned char c)
{
	/* Wait for UART to become ready to transmit */
	while (1) {
		if (mmio_read((uint64_t)&kernel_aux_base[AUX_MU_LSR_REG]) & (1 << 5))
			break;
	}
	mmio_write((uint64_t)&kernel_aux_base[AUX_MU_IO_REG], c);
	/*
	 * AUX_MU_LSR_REG shows the data status.
	 * If bit 5 is set the transmit FIFO can accept at least 1 byte.
	 */
}

/**
 * Gets a byte to the UART (serial input).
 * @return The byte received from the UART.
 * @details First waits for the UART to have received something,
 * and then reads the contents of the @ref AUX_MU_IO_REG register
 * @see peripherals/aux.h
 */
unsigned char uart_getc()
{
	/* Wait for UART to have received something */
	while (1) {
		if (mmio_read((uint64_t)&kernel_aux_base[AUX_MU_LSR_REG]) & (1 << 0))
			break;
	}
	return mmio_read((uint64_t)&kernel_aux_base[AUX_MU_IO_REG]);
	/*
	 * AUX_MU_LSR_REG shows the data status.
	 * If bit 0 is set the receive FIFO holds at least 1 symbol.
	 */
}

/**
 * Sends a string to the UART (serial output).
 * @param str A string pointer for the string to send to the UART.
 * @details Iterates over the string and prints each char to the serial output.
 * @note If a new New Line (`\n`) char is found, it appends a Carriage Return (`\r`) char,
 * so there is no need to include the `\r` char in the @a str string.
 * @see uart_putc()
 */
void uart_puts(const char* str)
{
	for (size_t i = 0; str[i] != '\0'; i++) {
		if (str[i] == '\n')
			uart_putc('\r');
		uart_putc((unsigned char)str[i]);
	}
}

/**
 * Gets a string from the UART (serial input).
 * @return A pointer to a static char array, that the string is saved.
 * @details Reads in a loop a byte from the serial input using the uart_getc()
 * function until a New Line (`\n`) or a Carriage Return (`\r`) char is received.
 * Echoes back each char to the serial output.
 * @note Gets up to @ref MAX_INPUT_LENGTH chars.
 * @note Always appends a null terminator (`\0`) at the end of the string.
 * @see uart_getc(), MAX_INPUT_LENGTH
 */
char *uart_gets()
{
	static char str[MAX_INPUT_LENGTH + 1];
	int i = 0;

	/* Initialize input string with null terminators */
	memset(&str, '\0', MAX_INPUT_LENGTH + 1);

	/* Get up to console's maximum length chars */
	for (i = 0; i < MAX_INPUT_LENGTH; i++) {
		/* Get char from serial, echo back */
		str[i] = (char) uart_getc();
		uart_putc(str[i]);
		/* If we get a NL or CR, break */
		if (str[i] == '\r' || str[i] == '\n') {
			break;
		}
	}
	/* Always append a null terminator at end of string */
	str[i] = '\0';

	return str;
}

/**
 * \ingroup exceptions
 * Exception handler for the UART (Asynchronous read from the serial input).
 * @details Gets called when an interrupt is triggered from the UART.
 * While the receiver holds a valid bit, reads the byte and echoes back.
 * @note If a Carriage Return (`\r`) char is found, it appends a New Line (`\n`) char.
 * @see handle_irq(), uart_getc(), uart_putc()
 */
void handle_uart_irq()
{
	unsigned char c;

	/* While the receiver holds a valid bit (on Read) */
	while ((mmio_read((uint64_t)&kernel_aux_base[AUX_MU_IIR_REG]) & (1 << 2)) == (1 << 2)) {
		c = uart_getc();
		if (c == '\r') {
			/* When user presses Enter a CR is returned */
			uart_putc(c);
			uart_putc('\n');
		}
		else {
			uart_putc(c);
		}
	}
}