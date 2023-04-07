#include <armv8-a/aarch64.h>

/**
 * \ingroup drivers
 * Memory-Mapped I/O output.
 * @param reg 64-bit register address.
 * @param data 32-bit data.
 * @details Writes the @a data value to the register at @a reg address.
*/
void mmio_write(uint64_t reg, uint32_t data)
{
	*(volatile uint32_t*)reg = data;
}

/**
 * \ingroup drivers
 * Memory-Mapped I/O input.
 * @param reg 64-bit register address.
 * @return 32-bit data.
 * @details Reads register at @a reg address and returns its @a data.
*/
uint32_t mmio_read(uint64_t reg)
{
	return *(volatile uint32_t*)reg;
}

/**
 * \ingroup drivers
 * Delays @a count of clock cycles.
 * @param count Number of cycles.
 * Loop <count> times, so the compiler won't optimize away
*/
void delay(int32_t count)
{
	asm volatile("__delay_%=:\n"
			"subs %[count], %[count], #1\n"
			"bne __delay_%=\n"
			: "=r" (count)
			: [count] "0" (count)
			: "cc"
		);
}