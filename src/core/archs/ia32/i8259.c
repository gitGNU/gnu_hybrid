/*
 * Copyright (C) 2008 Francesco Salvestrini
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "core/arch/port.h"
#include "core/arch/asm.h"
#include "core/arch/i8259.h"

#if CONFIG_I8259_DEBUG
#define dprintf(F,A...) printf("i8259: " F,##A)
#else
#define dprintf(F,A...)
#endif

#define SLAVE_IRQ    8
#define MASTER_SLAVE 2
#define ICU0         0x20
#define ICU1         0xA0
#define ICU_RESET    0x11

static uint16_t irq_mask;

static int remap(uint8_t master,
		 uint8_t slave)
{
	dprintf("Reprogramming master 0x%x / slave 0x%x\n");

	/* Send ICW1 */
	port_out8(ICU0,     ICU_RESET);
	port_out8(ICU1,     ICU_RESET);

	/* ICW2: Send the PIC indices in the IDT */
	port_out8(ICU0 + 1, master);
	port_out8(ICU1 + 1, slave);

	/* ICW3: Sets which PIC is the master */
	port_out8(ICU0 + 1, 0x04);
	port_out8(ICU1 + 1, 0x02);

	/* If the lowest bit is set then the PIC is working in the x86 arch */
	port_out8(ICU0 + 1, 0x01);
	port_out8(ICU1 + 1, 0x01);

	/* Disable all IRQs */
	port_out8(ICU0 + 1, 0xFF);
	port_out8(ICU1 + 1, 0xFF);

	irq_mask = 0xFFFF;

	return 1;
}

int i8259_init(void)
{
	dprintf("Initializing\n");

	cli();

	irq_mask = 0;

	return remap(0x20, 0x28);
}

void i8259_fini(void)
{
	dprintf("Finalizing\n");

	cli();
}

void i8259_irq_enable(int irq)
{
	dprintf("Enabling irq %d\n", irq);

	irq_mask = irq_mask & ~(1 << irq);
	if (irq >= 8) {
		irq_mask = irq_mask & (1 << 2);
	}

	port_out8(ICU0 + 1, irq_mask & 0xFF);
	port_out8(ICU1 + 1, (irq_mask >> 8) & 0xFF);
}

void i8259_irq_disable(int irq)
{
	dprintf("Disabling irq %d\n", irq);

	irq_mask = irq_mask | (1 << irq);
	if ((irq_mask & 0xFF00) == 0xFF00) {
		irq_mask = irq_mask | (1 << 2);
	}

	port_out8(ICU0 + 1, irq_mask & 0xFF);
	port_out8(ICU1 + 1, (irq_mask >> 8) & 0xFF);
}

uint16_t i8259_irq_mask_get(void)
{
	return irq_mask;
}

void i8259_irq_mask_set(uint16_t mask)
{
	dprintf("Setting irq mask to 0x%x\n", mask);

	irq_mask = mask;

	port_out8(ICU0 + 1, irq_mask & 0xFF);
	port_out8(ICU1 + 1, (irq_mask >> 8) & 0xFF);
}

static int irqs_enabled;

void arch_irqs_enable(void)
{
	i8259_irq_enable(0);
	irqs_enabled = 1;
}

int arch_irqs_enabled(void)
{
	return irqs_enabled;
}

void arch_irqs_disable(void)
{
	i8259_irq_disable(0);
	irqs_enabled = 0;
}

void arch_irqs_save(irq_flags_t* flags)
{
	uint16_t mask;

	unused_argument(flags);

	mask = i8259_irq_mask_get();
}

void arch_irqs_restore(const irq_flags_t* flags)
{
	unused_argument(flags);

	i8259_irq_mask_set(0);
}
