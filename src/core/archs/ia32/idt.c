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
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "core/arch/idt.h"
#include "core/arch/gdt.h"
#include "core/arch/asm.h"
#include "core/dbg/panic.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"

#if CONFIG_IDT_DEBUG
#define dprintf(F,A...) printf("idt: " F,##A)
#else
#define dprintf(F,A...)
#endif

#define IDT_ADDR     0x180000
#define IDT_ENTRIES  0x100       /* 256 */

#define IDT_PRESENT  0x8000      /* 10000000 00000000b */
#define IDT_TRAP     0x0700      /* 00000111 00000000b */
#define IDT_INT      0x0600      /* 00000110 00000000b */
#define IDT_TASK     0x0500      /* 00000101 00000000b */
#define IDT_32       0x0800      /* 00001000 00000000b */
#define IDT_DPL0     0x0000      /* 00000000 00000000b */
#define IDT_DPL1     0x2000      /* 00100000 00000000b */
#define IDT_DPL2     0x4000      /* 01000000 00000000b */
#define IDT_DPL3     0x6000      /* 01100000 00000000b */

typedef struct {
	uint16_t offset15_0;
	uint16_t segment;     /* Selector */
	uint16_t flags;       /* parms: 5
			       * zeros: 3
			       * type:  4
			       * azero: 1
			       * dpl:   2
			       * valid: 1
			       */
	uint16_t offset31_16;
} idt_descriptor_t;

idt_descriptor_t* idt_table = (idt_descriptor_t *) IDT_ADDR;

static void gate_set(uint32_t index,
		     uint16_t segment,
		     uint32_t offset,
		     uint16_t flags)
{
	assert(index < IDT_ENTRIES);
	idt_table[index].segment     = segment;
	idt_table[index].flags       = flags;
	idt_table[index].offset15_0  = offset & 0xFFFF;
	idt_table[index].offset31_16 = (offset >> 16) & 0xFFFF;
}

void idt_interrupt_set(uint32_t index,
		       void*    addr)
{
	gate_set(index, KERNEL_CS, (unsigned int) addr,
		 IDT_PRESENT | IDT_32 | IDT_INT | IDT_DPL3);
}

void idt_trap_set(uint32_t index,
		  void*    addr)
{
	gate_set(index, KERNEL_CS, (unsigned int) addr,
		 IDT_PRESENT | IDT_32 | IDT_TRAP | IDT_DPL3);
}

void idt_task_set(uint32_t index,
		  uint16_t segment)
{
	gate_set(index, segment, 0,
		 IDT_PRESENT | IDT_TASK | IDT_DPL0);
}

static void trap_default(void)
{
	panic("Unexpected interrupt");
}

#if CONFIG_DEBUGGER
static dbg_result_t command_idt_on_execute(FILE* stream,
					   int   argc,
					   char* argv[])
{
	int i;

	assert(stream);

	unused_argument(argc);
	unused_argument(argv);

	fprintf(stream, "IDT:\n");

	for (i = 0; i < IDT_ENTRIES; i++) {
		if (idt_table[i].flags & IDT_PRESENT) {
			fprintf(stream,
				"  %d    0x%02x / 0x%02x / 0x04%x%04x\n",
				i,
				idt_table[i].segment,
				idt_table[i].flags,
				idt_table[i].offset31_16,
				idt_table[i].offset15_0);
		}
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(idt,
		    "Show idt",
		    NULL,
		    NULL,
		    command_idt_on_execute,
		    NULL);
#endif

int idt_init(void)
{
#if 0
	struct desc_p idt_p;
#endif

	int           i;

	/* Fill all vectors with default handler */
	for (i = 0; i < IDT_ENTRIES; i++) {
		idt_trap_set(i, trap_default);
	}

#if 0
	/* Setup trap handlers */
	for (i = 0; i < NR_TRAP; i++) {
		idt_set(i, trap_table[i]);
	}

	/* Setup interrupt handlers */
	for (i = 0; i < 16; i++) {
		idt_interrupt_set(0x20 + i, intr_table[i]);
	}

	/* Setup debug trap */
	idt_set(3, trap_3, KERNEL_CS, ST_USER | ST_TRAP_GATE);

	/* Setup system call handler */
	idt_set(SYSCALL_INT, syscall_entry, KERNEL_CS, ST_USER | ST_TRAP_GATE);

	/* Load IDT */
	idt_p.limit = sizeof(idt) - 1;
	idt_p.base  = (u_long)&idt;
	lidt(&idt_p.limit);
#endif

	return 1;
}

void idt_fini(void)
{
	missing();
}
