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
#include "arch/idt.h"
#include "arch/gdt.h"
#include "arch/asm.h"
#include "arch/i8259.h"
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

static void idt_gate_set(uint32_t i,
			 uint16_t flags,
			 uint32_t offset)
{
	assert(i < IDT_ENTRIES);

	idt_table[i].segment     = SEGMENT_BUILDER(0,0,SEGMENT_KERNEL_CODE);
	idt_table[i].flags       = flags | IDT_PRESENT | IDT_DPL0 | IDT_32;
	idt_table[i].offset31_16 = (offset >> 16) & 0xFFFF;
	idt_table[i].offset15_0  = offset & 0xFFFF;

#if 0
	if (idt_table[i].flags & IDT_PRESENT) {
		dprintf("gate %d: "
			"offset=0x%04x%04x "
			"segment=0x%04x, "
			"flags=0x%04x "
			"%s\n",
			i,
			idt_table[i].offset31_16, idt_table[i].offset15_0,
			idt_table[i].segment,
			idt_table[i].flags,
			((flags & IDT_IRQ) ? "IRQ" :
			 ((flags & IDT_TRAP) ? "TRAP" :
			  "?")));
	}
#endif
}

static void idt_gate_clear(uint32_t i)
{
	assert(i < IDT_ENTRIES);

	idt_table[i].segment     = SEGMENT_BUILDER(0,0,SEGMENT_NULL);
	idt_table[i].flags       = IDT_INT | IDT_32;
	idt_table[i].offset31_16 = 0;
	idt_table[i].offset15_0  = 0;
}

static void idt_interrupt_set(uint32_t i,
			      void *   addr)
{
	idt_gate_set(i, IDT_INT, (uint32_t) addr);
}

static void idt_trap_set(uint32_t i,
			 void *   addr)
{
	idt_gate_set(i, IDT_TRAP, (uint32_t) addr);
}

static void idt_load(void)
{
	idt_pointer_t idt_p;

	idt_p.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
	idt_p.base  = (uint32_t) idt_table;

	dprintf("Loading IDT table at 0x%p (%d entries)\n",
		idt_table, IDT_ENTRIES);
	lidt(&idt_p.limit);
}

void idt_frame_dump(regs_t * regs)
{
	uint_t ss, esp;

	if (regs->cs & 3) {
		ss  = regs->ss;
		esp = regs->esp;
	} else {
		ss  = regs->ds;
		esp = (uint_t) regs;
	}

	printf("Frame dump (isr %d):\n", regs->isr_no);
	printf("  eax 0x%08x ebx 0x%08x ecx 0x%08x edx 0x%08x\n",
	       regs->eax, regs->ebx, regs->ecx, regs->edx);
	printf("  esi 0x%08x edi 0x%08x\n",
	       regs->esi, regs->edi);
	printf("  eip 0x%08x esp 0x%08x ebp 0x%08x eflags 0x%08x\n",
	       regs->eip, esp, regs->ebp, regs->eflags);
	printf("  cs  0x%08x ss  0x%08x ds  0x%08x es     0x%08x\n",
	       regs->cs, ss, regs->ds, regs->es);
}

extern void trap_00(void);
extern void trap_01(void);
extern void trap_02(void);
extern void trap_03(void);
extern void trap_04(void);
extern void trap_05(void);
extern void trap_06(void);
extern void trap_07(void);
extern void trap_08(void);
extern void trap_09(void);
extern void trap_10(void);
extern void trap_11(void);
extern void trap_12(void);
extern void trap_13(void);
extern void trap_14(void);
extern void trap_15(void);
extern void trap_16(void);
extern void trap_17(void);
extern void trap_18(void);
extern void trap_19(void);
extern void trap_20(void);
extern void trap_21(void);
extern void trap_22(void);
extern void trap_23(void);
extern void trap_24(void);
extern void trap_25(void);
extern void trap_26(void);
extern void trap_27(void);
extern void trap_28(void);
extern void trap_29(void);
extern void trap_30(void);
extern void trap_31(void);

extern void irq_00(void);
extern void irq_01(void);
extern void irq_02(void);
extern void irq_03(void);
extern void irq_04(void);
extern void irq_05(void);
extern void irq_06(void);
extern void irq_07(void);
extern void irq_08(void);
extern void irq_09(void);
extern void irq_10(void);
extern void irq_11(void);
extern void irq_12(void);
extern void irq_13(void);
extern void irq_14(void);
extern void irq_15(void);

void idt_clear(void)
{
	int i;

	/* Clear all gates */
	for (i = 0; i < IDT_ENTRIES; i++) {
		idt_gate_clear(i);
	}
}

int idt_init(void)
{
	idt_clear();

	idt_trap_set(0,  trap_00);
	idt_trap_set(1,  trap_01);
	idt_trap_set(2,  trap_02);
	idt_trap_set(3,  trap_03);
	idt_trap_set(4,  trap_04);
	idt_trap_set(5,  trap_05);
	idt_trap_set(6,  trap_06);
	idt_trap_set(7,  trap_07);
	idt_trap_set(8,  trap_08);
	idt_trap_set(9,  trap_09);
	idt_trap_set(10, trap_10);
	idt_trap_set(11, trap_11);
	idt_trap_set(12, trap_12);
	idt_trap_set(13, trap_13);
	idt_trap_set(14, trap_14);
	idt_trap_set(15, trap_15);
	idt_trap_set(16, trap_16);
	idt_trap_set(17, trap_17);
	idt_trap_set(18, trap_18);
	idt_trap_set(19, trap_19);
	idt_trap_set(20, trap_20);
	idt_trap_set(21, trap_21);
	idt_trap_set(22, trap_22);
	idt_trap_set(23, trap_23);
	idt_trap_set(24, trap_24);
	idt_trap_set(25, trap_25);
	idt_trap_set(26, trap_26);
	idt_trap_set(27, trap_27);
	idt_trap_set(28, trap_28);
	idt_trap_set(39, trap_29);
	idt_trap_set(30, trap_30);
	idt_trap_set(31, trap_31);

	idt_interrupt_set(32, irq_01);
	idt_interrupt_set(33, irq_02);
	idt_interrupt_set(34, irq_03);
	idt_interrupt_set(35, irq_04);
	idt_interrupt_set(36, irq_05);
	idt_interrupt_set(37, irq_06);
	idt_interrupt_set(38, irq_07);
	idt_interrupt_set(39, irq_08);
	idt_interrupt_set(40, irq_09);
	idt_interrupt_set(41, irq_10);
	idt_interrupt_set(42, irq_11);
	idt_interrupt_set(43, irq_12);
	idt_interrupt_set(44, irq_13);
	idt_interrupt_set(45, irq_14);
	idt_interrupt_set(46, irq_15);

	idt_load();

	return 1;
}

void idt_fini(void)
{
	idt_clear();
	idt_load();
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
				"  %d     0x04%x%04x/0x%04x/0x%04x\n",
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
