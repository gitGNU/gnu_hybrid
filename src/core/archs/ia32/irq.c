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
#include "core/arch/idt.h"
#include "core/arch/irq.h"
#include "core/arch/port.h"
#include "core/arch/idt.h"
#include "core/arch/arch.h"
#include "core/dbg/debug.h"
#include "core/arch/i8259.h"
#include "core/arch/asm.h"

#if CONFIG_IRQ_DEBUG
#define dprintf(F,A...) printf("irq: " F,##A)
#else
#define dprintf(F,A...)
#endif

static irq_handler_t handlers[I8259_IRQS];

void irq_handler_install(uint_t        irq,
			 irq_handler_t handler)
{
	assert(irq < I8259_IRQS);
	assert(handler);

	dprintf("Handler 0x%p installed to irq %d\n", handler, irq);

	handlers[irq] = handler;
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

	printf("IRQ %d!\n", vector);
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
	}

	irq_level = prio_old;
	i8259_mask_set(mask_table[irq_level]);
}

static int irqs_enabled;

int irq_init(void)
{
	int i;

	irqs_enabled = 0;

	for (i = 0; i < I8259_IRQS; i++) {
		handlers[i]   = NULL;
		prio_table[i] = 0;
		mask_table[i] = 0xFFFB;
	}

	if (!i8259_init()) {
		panic("Cannot initialize i8259");
	}

	return 1;
}

void irq_fini(void)
{
	i8259_fini();
}

void arch_irqs_enable(void)
{
	i8259_enable(0);
	irqs_enabled = 1;
}

int arch_irqs_enabled(void)
{
	return irqs_enabled;
}

void arch_irqs_disable(void)
{
	i8259_disable(0);
	irqs_enabled = 0;
}

arch_irqs_state_t arch_irqs_state_get(void)
{
	return i8259_mask_get();
}

void arch_irqs_state_set(arch_irqs_state_t * state)
{
	assert(state);

	i8259_mask_set(*state);
}
