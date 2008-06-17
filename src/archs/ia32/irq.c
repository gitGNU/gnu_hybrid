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

/*
 * NOTE on masking and unmasking:
 *     A high value prevents the interrupt, a low value allows the interrupt
 */
void irq_unmask(uint_t irq)
{
	dprintf("Unmasking irq %d\n", irq);
	i8259_enable(irq);
	//i8259_mask_set(i8259_mask_get() & ((i8259_mask_t) ~(1 << irq)));
}

void irq_mask(uint_t irq)
{
	dprintf("Masking irq %d\n", irq);
	i8259_disable(irq);
	//i8259_mask_set(i8259_mask_get() | ((i8259_mask_t) (1 << irq)));
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
	//i8259_enable(irq);
	//irq_unmask(irq);

	dprintf("Handler 0x%p installed on irq %d\n", handlers[irq], irq);

	return 1;
}

void irq_handler_uninstall(uint_t irq)
{
	assert(irq < I8259_IRQS);

	//i8259_disable(irq);
	//irq_mask(irq);
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

	dprintf("Handler running for IRQ %d/%d/%d (mask = 0x%x)\n",
		regs->isr_no, irq, I8259_IRQS, i8259_mask_get());
	idt_frame_dump(regs);

	assert((irq >= 0) && (irq < I8259_IRQS));

	/* Deassert before acknowledging, in order to avoid spurious */
	irq_mask(irq);

	/* Acknowledge PIC */
	i8259_eoi(irq);

	handler = handlers[irq];
	if (handler) {
		handler(regs);
	} else {
		dprintf("No handler installed for interrupt %d\n", irq);
	}

	irq_unmask(irq);
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

#if 0
static void keyboard(regs_t * regs)
{
	dprintf("KEYBOARD (0x%p)\n", regs);
}
#endif

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

	dprintf("Current mask 0x%x\n", i8259_mask_get());
	for (i = 0; i < I8259_IRQS; i++) {
		irq_mask(i);
	}
	dprintf("Current mask 0x%x\n", i8259_mask_get());

	return 1;
}

int irq_handlers_install(void)
{
	irq_disable();

	if (!irq_handler_install(0, timer)) {
		return 0;
	}
	irq_unmask(0);

#if 0
	if (!irq_handler_install(1, keyboard)) {
		return 0;
	}
	irq_unmask(1);
#endif

	irq_enable();

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
