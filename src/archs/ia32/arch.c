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
#include "libc/stdio.h"
#include "libc/assert.h"
#include "archs/arch.h"
#include "archs/ia32/asm.h"
#include "archs/ia32/port.h"
#include "archs/ia32/cpu.h"
#include "archs/ia32/gdt.h"
#include "archs/ia32/tss.h"
#include "archs/ia32/idt.h"
#include "archs/ia32/bios.h"
#include "archs/ia32/cmos.h"
#include "archs/ia32/i8253.h"
#include "archs/ia32/dma.h"
#include "archs/ia32/irq.h"

#if CONFIG_ARCH_DEBUG
#define dprintf(F,A...) printf("arch: " F,##A)
#else
#define dprintf(F,A...)
#endif

int arch_init(void)
{
	dprintf("Initializing architecture\n");

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

	if (!irq_handlers_install()) {
		panic("Cannot install IRQ handlers");
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
	irq_fini();
	i8253_fini();
	tss_fini();
	idt_fini();
	gdt_fini();
	cpus_fini();
	cmos_fini();
	bios_fini();
}
