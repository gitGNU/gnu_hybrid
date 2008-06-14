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
#include "archs/ia32/irq.h"
#include "archs/ia32/port.h"
#include "archs/ia32/idt.h"
#include "archs/ia32/arch.h"
#include "archs/ia32/i8259.h"
#include "archs/ia32/asm.h"
#include "libs/debug.h"

#if CONFIG_ARCH_IRQ_DEBUG
#define dprintf(F,A...) printf("irq: " F,##A)
#else
#define dprintf(F,A...)
#endif

static irq_handler_t handlers[I8259_IRQS];

void irq_unmask(uint_t irq)
{
	i8259_mask_set(i8259_mask_get() & ((uint_t) ~(1 << irq)));
}

void irq_mask(uint_t irq)
{
	i8259_mask_set(i8259_mask_get() & ((uint_t) (1 << irq)));
}

int irq_handler_install(uint_t        irq,
			irq_handler_t handler,
			int           shared)
{
	assert(irq < I8259_IRQS);
	assert(handler);

	if (handlers[irq]) {
		dprintf("Handler 0x%p present on irq %d\n", handlers[irq], irq);
		return 0;
	}

	handlers[irq] = handler;
	i8259_setup(irq, shared ? I8259_TYPE_LEVEL : I8259_TYPE_EDGE);
	i8259_enable(irq);

	dprintf("Handler 0x%p installed on irq %d\n", handlers[irq], irq);

	return 1;
}

void irq_handler_uninstall(uint_t irq)
{
	assert(irq < I8259_IRQS);

	i8259_disable(irq);
	handlers[irq] = NULL;
	dprintf("Handler for irq %d uninstalled\n", irq);
}

void irq_enable(void)
{
	sti();
}

void irq_disable(void)
{
	cli();
}

void irq_handler(regs_t * regs)
{
	irq_handler_t handler;
	int           irq;

	/* NOTE: We reach this point with interrupts disabled */
	assert(regs);

	irq = regs->isr_no - I8259_IDT_BASE_INDEX;

	dprintf("IRQ %d/%d/%d (mask = 0x%x)\n",
		regs->isr_no, irq, I8259_IRQS, i8259_mask_get());
	idt_frame_dump(regs);

	assert((irq >= 0) && (irq < I8259_IRQS));

	/* Deassert before acknowledging, in order to avoid spurious */
	irq_unmask(irq);

	/* Acknowledge PIC */
	i8259_eoi(irq);

	handler = handlers[irq];
	if (handler) {
		handler(regs);
	} else {
		dprintf("No handler installed for interrupt %d\n", irq);
	}

	irq_mask(irq);
}

arch_irqs_state_t irq_state_get(void)
{
	return i8259_mask_get();
}

void irq_state_set(arch_irqs_state_t * state)
{
	i8259_mask_set(*state);
}

static void timer(regs_t * regs)
{
	dprintf("TIMER (0x%p)\n", regs);
}

static void keyboard(regs_t * regs)
{
	dprintf("KEYBOARD (0x%p)\n", regs);
}

int irq_init(void)
{
	int i;

	irq_disable();

	for (i = 0; i < I8259_IRQS; i++) {
		handlers[i]   = NULL;
	}

	if (!i8259_init()) {
		dprintf("Cannot initialize i8259");
		return 0;
	}

	if (!irq_handler_install(0, timer, 0)) {
		return 0;
	}
	irq_unmask(0);

	if (!irq_handler_install(1, keyboard, 0)) {
		return 0;
	}
	irq_unmask(1);

	return 1;
}

void irq_fini(void)
{
	int i;

	irq_disable();

	for (i = 0; i < I8259_IRQS; i++) {
		/* irq_mask(i); */
		irq_handler_uninstall(i);
	}

	i8259_fini();
}
