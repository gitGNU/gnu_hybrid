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

#define IDT_DPL_MASK (IDT_DPL3 | IDT_DPL2 | IDT_DPL1 | IDT_DPL0)

struct idt_entry {
	uint16_t offset15_0;  /* Base-LO */
	uint16_t segment;     /* Selector */
	uint16_t flags;       /* reserved: 5
			       * flags:    3
			       * type:     3
			       * size:     1
			       * zero:     1
			       * dpl:      2
			       * present:  1
			       */
	uint16_t offset31_16; /* Base-HI */
} ATTRIBUTE(packed);
typedef struct idt_entry idt_entry_t;

struct idt_pointer {
	uint16_t limit;
	uint32_t base;
} ATTRIBUTE(packed);
typedef struct idt_pointer idt_pointer_t;

static idt_entry_t idt_table[IDT_ENTRIES];

static void idt_gate_set(uint32_t index,
			 uint16_t flags,
			 uint32_t offset)
{
	assert(index < IDT_ENTRIES);

	idt_table[index].segment     = SEGMENT_BUILDER(0,0,SEGMENT_KERNEL_CODE);
	idt_table[index].flags       = flags | IDT_PRESENT | IDT_DPL3 | IDT_32;
	idt_table[index].offset15_0  = offset & 0xFFFF;
	idt_table[index].offset31_16 = (offset >> 16) & 0xFFFF;
}

static void idt_gate_clear(uint32_t index)
{
	assert(index < IDT_ENTRIES);

	idt_table[index].segment     = SEGMENT_BUILDER(0,0,SEGMENT_KERNEL_CODE);
	idt_table[index].flags       = IDT_32 | IDT_INT;
	idt_table[index].offset15_0  = 0;
	idt_table[index].offset31_16 = 0;
}

static void idt_interrupt_set(uint32_t index,
		       void *   addr)
{
	idt_gate_set(index, IDT_INT, (uint32_t) addr);
}

#if 0
static void idt_trap_set(uint32_t index,
		  void *   addr)
{
	idt_gate_set(index, IDT_TRAP, (uint32_t) addr);
}
#endif

static void idt_load(idt_entry_t * table,
		     size_t        entries)
{
	idt_pointer_t idt_p;

	assert(entries <= IDT_ENTRIES);

	idt_p.limit = (sizeof(idt_entry_t) * entries) - 1;
	idt_p.base  = (uint32_t) table;

	lidt(&idt_p.limit);
}

static void irq0(void)
{
	printf("TEST ");
}

int idt_init(void)
{
	int i;

	/* Clear all gates */
	for (i = 0; i < IDT_ENTRIES; i++) {
		idt_gate_clear(i);
	}
	/* Set default gates */
	idt_interrupt_set(0, irq0);

	idt_load(idt_table, IDT_ENTRIES);

	return 1;
}

void idt_fini(void)
{
	missing();
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
				"  %d     0x04%x%04x / 0x%04x /0x%04x\n",
				i,
				idt_table[i].offset31_16,
				idt_table[i].offset15_0,
				idt_table[i].segment,
				idt_table[i].flags);
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
