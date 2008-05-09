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

#include "debug.h"
#include "libc/stdio.h"

#include "config/config.h"
#include "core/archs/arch.h"
#include "core/arch/port.h"
#include "core/arch/cpu.h"
#include "core/arch/gdt.h"
#include "core/arch/idt.h"
#include "core/arch/bios.h"
#include "core/arch/cmos.h"
#include "core/arch/i8253.h"
#include "core/arch/i8259.h"

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
	/* We know CPU capabilities now */

	/* Turn IRQ off before initializing GDT */
	if (!i8259_init()) {
		panic("Cannot initialize i8259");
	}

	if (!gdt_init()) {
		panic("Cannot initialize GDT");
	}

	if (!idt_init()) {
		panic("Cannot initialize IDT");
	}

	i8259_irq_enable(0);

	if (!i8253_init()) {
		panic("Cannot initialize i8253");
	}

	/* We can call delay() now */

	dprintf("Architecture initialized\n");

	return 1;
}

void arch_fini(void)
{
	/* XXX FIXME: Review the shutdown procedure ... */
	i8253_fini();

	i8259_irq_disable(0);

	i8259_fini();
	idt_fini();
	gdt_fini();
	cpus_fini();
	cmos_fini();
	bios_fini();
}
