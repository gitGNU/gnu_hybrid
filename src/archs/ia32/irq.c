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
#include "arch/irq.h"
#include "arch/port.h"
#include "arch/idt.h"
#include "arch/arch.h"
#include "arch/i8259.h"
#include "arch/asm.h"
#include "core/dbg/debug.h"

#if CONFIG_ARCH_IRQ_DEBUG
#define dprintf(F,A...) printf("irq: " F,##A)
#else
#define dprintf(F,A...)
#endif

static irq_handler_t handlers[I8259_IRQS];

int irq_handler_install(uint_t        irq,
			irq_handler_t handler)
{
	assert(irq < I8259_IRQS);
	assert(handler);

	dprintf("Handler 0x%p installed to irq %d\n", handler, irq);

	if (handlers[irq]) {
		dprintf("Handler is present on irq %d\n", irq);
		return 0;
	}
	handlers[irq] = handler;

	return 1;
}

void irq_handler_uninstall(uint_t irq)
{
	assert(irq < I8259_IRQS);

	dprintf("IRQ %d handler uninstalled\n", irq);

	handlers[irq] = NULL;
}

static int          irq_level;
static int          prio_table[I8259_IRQS];
static i8259_mask_t mask_table[I8259_IRQS];

void irq_handler(regs_t * regs)
{
	irq_handler_t handler;
	int           prio_old;
	int           prio_new;
	int           vector;

	assert(regs);

	vector = regs->isr_no - I8259_IDT_BASE_INDEX;
	assert((vector >= 0) && (vector < I8259_IRQS));

	dprintf("IRQ %d/%d/%d (mask = 0x%x)\n",
		regs->isr_no, vector, I8259_IRQS, i8259_mask_get());
	idt_frame_dump(regs);

	prio_old = irq_level;
	prio_new = prio_table[vector];
	if (prio_new > prio_old) {
		irq_level = prio_new;
	}
	i8259_mask_set(mask_table[irq_level]);

	i8259_eoi(vector);

	handler = handlers[vector];
	if (handler) {
		sti();
		handler(regs);
		cli();
	} else {
		dprintf("No handler installed for interrupt %d\n", vector);
	}

	irq_level = prio_old;
	i8259_mask_set(mask_table[irq_level]);
}

static void timer(regs_t * regs)
{
	dprintf("TICKER (%p) !!!\n", regs);
}

void irq_enable(void)
{
	i8259_enable(0);
	sti();
}

void irq_disable(void)
{
	cli();
	i8259_disable(0);
}

arch_irqs_state_t irq_state_get(void)
{
	return i8259_mask_get();
}

void irq_state_set(arch_irqs_state_t * state)
{
	i8259_mask_set(*state);
}

int irq_init(void)
{
	int i;

	irq_level = 0;

	for (i = 0; i < I8259_IRQS; i++) {
		handlers[i]   = NULL;
		prio_table[i] = 0;
		mask_table[i] = 0xFFFB;	/* Disable all, except the cascade */
	}

	if (!i8259_init()) {
		dprintf("Cannot initialize i8259");
		return 0;
	}

	if (!irq_handler_install(0, timer)) {
		return 0;
	}

	return 1;
}

void irq_fini(void)
{
	i8259_fini();
}
