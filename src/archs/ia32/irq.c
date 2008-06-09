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
#include "core/dbg/debug.h"

#if CONFIG_ARCH_IRQ_DEBUG
#define dprintf(F,A...) printf("irq: " F,##A)
#else
#define dprintf(F,A...)
#endif

static irq_handler_t handlers[I8259_IRQS];
static int           prio_current;
static int           prio_table[I8259_IRQS]; /* Vector -> Priority */
static i8259_mask_t  mask_table[I8259_PRIO]; /* Priority -> Mask */

void irq_unmask(uint_t irq,
		int    level)
{
	int    i;
	uint_t unmask;

	unmask = (uint_t) ~(1 << irq);

	prio_table[irq] = level;

	for (i = 0; i < level; i++) {
		mask_table[i] &= unmask;
	}

	i8259_mask_set(mask_table[prio_current]);
}

void irq_mask(uint_t irq)
{
	int    i, prio;
	uint_t mask;

	mask = (uint_t)(1 << irq);

	prio = prio_table[irq];
	for (i = 0; i < prio; i++) {
		mask_table[i] |= mask;
	}
	prio_table[irq] = 0;

	i8259_mask_set(mask_table[prio_current]);
}

int irq_handler_install(uint_t        irq,
			irq_handler_t handler)
{
	assert(irq < I8259_IRQS);
	assert(handler);

	if (handlers[irq]) {
		dprintf("Handler 0x%p present on irq %d\n", handlers[irq], irq);
		return 0;
	}

	handlers[irq] = handler;

	i8259_setup(irq, I8259_TYPE_TRIGGER);
	i8259_enable(irq);

	dprintf("Handler 0x%p installed to irq %d\n", handlers[irq], irq);

	return 1;
}

void irq_handler_uninstall(uint_t irq)
{
	assert(irq < I8259_IRQS);

	i8259_disable(irq);
	handlers[irq] = NULL;
	dprintf("IRQ %d handler uninstalled\n", irq);
}

void irq_handler(regs_t * regs)
{
	irq_handler_t handler;
	int           prio_old;
	int           prio_new;
	int           vector;

	assert(regs);

	vector = regs->isr_no - I8259_IDT_BASE_INDEX;

	dprintf("IRQ %d/%d/%d (mask = 0x%x)\n",
		regs->isr_no, vector, I8259_IRQS, i8259_mask_get());

	/* idt_frame_dump(regs); */

	assert((vector >= 0) && (vector < I8259_IRQS));

	prio_old = prio_current;
	prio_new = prio_table[vector];

	/* Ignore spurious interrupt */
	if (prio_new > prio_old) {
		prio_current = prio_new;
	}

	/* Deassert before acknowledging, in order to avoid spurious */
	i8259_mask_set(mask_table[prio_current]);

	/* Acknowledge PIC */
	i8259_eoi(vector);

	handler = handlers[vector];
	if (handler) {
		sti();
		handler(regs);
		cli();
	} else {
		dprintf("No handler installed for interrupt %d\n", vector);
	}

	/* Restore interrupt level */
	prio_current = prio_old;
	i8259_mask_set(mask_table[prio_current]);
}

void irq_enable(void)
{
	sti();
}

void irq_disable(void)
{
	cli();
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

	for (i = 0; i < I8259_IRQS; i++) {
		handlers[i]   = NULL;
		mask_table[i] = 0xFFFB;	/* Disable all, except the cascade */
	}

	prio_current = 0;
	for (i = 0; i < I8259_PRIO; i++) {
		prio_table[i] = 0;
	}

	if (!i8259_init()) {
		dprintf("Cannot initialize i8259");
		return 0;
	}

	if (!irq_handler_install(0, timer)) {
		return 0;
	}
	irq_unmask(0, 10);

	if (!irq_handler_install(1, keyboard)) {
		return 0;
	}
	irq_unmask(1, 9);

	return 1;
}

void irq_fini(void)
{
	int i;

	for (i = 0; i < I8259_IRQS; i++) {
		irq_mask(i);
		irq_handler_uninstall(i);
	}

	i8259_fini();
}
