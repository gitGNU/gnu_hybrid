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
#include "core/dbg/debug.h"
#include "core/arch/i8259.h"
#include "core/arch/asm.h"

static irq_handler_t irq_handlers[NR_IRQS];

void irq_handler_install(uint_t        irq,
			 irq_handler_t handler)
{
	assert(irq < 16);
	assert(handler);

	irq_handlers[irq] = handler;
}

void irq_handler_uninstall(uint_t irq)
{
	assert(irq < 16);

	irq_handlers[irq] = NULL;
}

static int      irq_level;
static int      prio_table[NR_IRQS];
static uint16_t mask_table[NR_IRQS];

void irq_handler(regs_t * regs)
{
	irq_handler_t handler;
	int           prio_old;
	int           prio_new;
	int           vector;

	assert(regs);

	//	assert(regs->isr_no >= 32);

	printf("IRQ %d!\n", regs->isr_no);
	idt_frame_dump(regs);

	vector   = regs->isr_no;
	prio_old = irq_level;
	prio_new = prio_table[vector];
	if (prio_new > prio_old) {
		irq_level = prio_new;
	}
	i8259_mask_set(mask_table[irq_level]);

	if (regs->isr_no >= 40) {
		i8259_eoi_slave();
	}
	i8259_eoi_master();

	handler = irq_handlers[regs->isr_no - 32];
	if (handler) {
		sti();
		handler(regs);
		cli();
	}

	irq_level = prio_old;
	i8259_mask_set(mask_table[irq_level]);
}

int irq_init(void)
{
	int i;

	for (i = 0; i < NR_IRQS; i++) {
		irq_handlers[i] = NULL;
	}

	return 1;
}

void irq_fini(void)
{
}
