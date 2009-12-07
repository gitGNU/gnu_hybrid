/* -*- c -*- */

/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
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
#include "archs/x86/irq.h"
#include "archs/x86/port.h"
#include "archs/x86/idt.h"
#include "archs/x86/i8259.h"
#include "archs/x86/asm.h"
#include "archs/arch.h"
#include "libs/debug.h"

#if CONFIG_ARCH_IRQ_DEBUG
#define dprintf(F,A...) printf("irq: " F,##A)
#else
#define dprintf(F,A...)
#endif

static arch_irq_handler_t handlers[I8259_IRQS];

/*
 * NOTE on masking and unmasking:
 *     A high value prevents the interrupt, a low value allows the interrupt
 */
void irq_unmask(uint_t irq)
{
        /* dprintf("Unmasking irq %d\n", irq); */
        i8259_enable(irq);
        /* i8259_mask_set(i8259_mask_get() & ((i8259_mask_t) ~(1 << irq))); */
}

void irq_mask(uint_t irq)
{
        /* dprintf("Masking irq %d\n", irq); */
        i8259_disable(irq);
        /* i8259_mask_set(i8259_mask_get() | ((i8259_mask_t) (1 << irq))); */
}

static int handler_install(uint_t             irq,
                           arch_irq_handler_t handler)
{
        assert(irq < I8259_IRQS);
        assert(handler);

        if (handlers[irq]) {
                dprintf("Handler already present on irq %d\n", irq);
                return 0;
        }

        handlers[irq] = handler;

        dprintf("Handler 0x%p installed on irq %d\n", handlers[irq], irq);

        return 1;
}

static int handler_uninstall(uint_t irq)
{
        assert(irq < I8259_IRQS);

        if (handlers[irq]) {
                handlers[irq] = NULL;
                dprintf("Handler for irq %d uninstalled\n", irq);
        } else {
                dprintf("No handler for irq %d to be uninstalled\n", irq);
        }

        return 1;
}

int arch_irq_handler_set(uint_t             irq,
                         arch_irq_handler_t handler)
{
        int retval;

        irq_mask(irq);

        if (handler) {
                return handler_install(irq, handler);
        } else {
                return handler_uninstall(irq);
        }

        irq_unmask(irq);

        return retval;
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
        arch_irq_handler_t handler;
        int                irq;

        /* NOTE: We reach this point with interrupts disabled */
        assert(regs);

#if 0
        irq = regs->isr_no - I8259_IDT_BASE_INDEX;
#else
        irq = regs->isr_no;
#endif

#if 0
        dprintf("Handler running for IRQ %d/%d/%d (mask = 0x%x)\n",
                regs->isr_no, irq, I8259_IRQS, i8259_mask_get());

        idt_frame_dump(regs);
#endif

        assert((irq >= 0) && (irq < I8259_IRQS));

        /* Deassert before acknowledging, in order to avoid spurious */
        irq_mask(irq);

        /* Acknowledge PIC */
        i8259_eoi(irq);

        handler = handlers[irq];
        if (handler) {
#if 0
                dprintf("Running handler 0x%p for interrupt %d\n",
                        handler, irq);
#endif
                handler(irq);
        } else {
#if 0
                dprintf("No handler installed for interrupt %d\n",
                        irq);
#endif
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

int irq_init(void)
{
        int i;

        irq_disable();

        for (i = 0; i < I8259_IRQS; i++) {
                handlers[i] = NULL;
        }

        if (!i8259_init()) {
                dprintf("Cannot initialize i8259\n");
                return 0;
        }

        dprintf("Current mask 0x%x\n", i8259_mask_get());
        for (i = 0; i < I8259_IRQS; i++) {
                irq_mask(i);
        }
        dprintf("Current mask 0x%x\n", i8259_mask_get());

        return 1;
}

void irq_fini(void)
{
        int i;

        irq_disable();

        for (i = 0; i < I8259_IRQS; i++) {
                /* irq_mask(i); */
                handler_uninstall(i);
        }

        i8259_fini();
}
