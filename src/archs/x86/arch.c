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
#include "libc/stdio.h"
#include "dbg/debug.h"
#include "archs/arch.h"
#include "archs/boot/bootram.h"
#include "archs/x86/asm.h"
#include "archs/x86/port.h"
#include "archs/x86/cpu.h"
#include "archs/x86/gdt.h"
#include "archs/x86/tss.h"
#include "archs/x86/idt.h"
#include "archs/x86/bios.h"
#include "archs/x86/cmos.h"
#include "archs/x86/i8253.h"
#include "archs/x86/dma.h"
#include "archs/x86/irq.h"
#include "archs/x86/tsc.h"

#if CONFIG_ARCH_DEBUG
#define dprintf(F,A...) printf("arch: " F,##A)
#else
#define dprintf(F,A...)
#endif

int arch_init(void)
{
        dprintf("Initializing architecture\n");

        /* XXX FIXME: Reserve lower memory (it is not assured!) */
        bootram_reserve(0, 1024 * 1024);

        if (!bios_init()) {
                panic("Cannot initialize BIOS");
        }

        if (!cmos_init()) {
                panic("Cannot initialize CMOS");
        }

        if (!cpus_init()) {
                panic("Cannot initialize CPU(s)");
        }

        if (!gdt_init()) {
                panic("Cannot initialize GDT");
        }

        if (!idt_init()) {
                panic("Cannot initialize IDT");
        }

        if (!tss_init()) {
                panic("Cannot initialize TSS");
        }

        if (!irq_init()) {
                panic("Cannot initialize IRQs");
        }
        /* IRQs initialized but still disabled */

        if (!i8253_init()) {
                panic("Cannot initialize i8253");
        }

        if (!tsc_init()) {
                panic("Cannot initialize tsc");
        }

        if (!dma_init()) {
                panic("Cannot initialize DMAs");
        }

        dprintf("Architecture initialized\n");

        return 1;
}

void arch_fini(void)
{
        dma_fini();
        tsc_fini();
        irq_fini();
        i8253_fini();
        tss_fini();
        idt_fini();
        gdt_fini();
        cpus_fini();
        cmos_fini();
        bios_fini();
}
