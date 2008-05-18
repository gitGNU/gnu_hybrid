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
#include "core/arch/port.h"
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
	idt_table[index].offset31_16 = (offset >> 16) & 0xFFFF;
	idt_table[index].offset15_0  = offset & 0xFFFF;
}

static void idt_gate_clear(uint32_t index)
{
	assert(index < IDT_ENTRIES);

	idt_table[index].segment     = SEGMENT_BUILDER(0,0,SEGMENT_KERNEL_CODE);
	idt_table[index].flags       = IDT_32 | IDT_INT;
	idt_table[index].offset31_16 = 0;
	idt_table[index].offset15_0  = 0;
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

/* This defines what the stack looks like after an ISR was running */
struct regs {
	/* pushed the segs last */
	uint_t gs, fs, es, ds;
	/* pushed by 'pusha' */
	uint_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	/* our 'push byte #' and ecodes do this */
	uint_t int_no, err_code;
	/* pushed by the processor automatically */
	uint_t eip, cs, eflags, useresp, ss;
};
typedef struct regs regs_t;

static char * exception_messages[] = {
	"Division By Zero Exception",
	"Debug Exception",
	"Non Maskable Interrupt Exception",
	"Breakpoint Exception",
	"Into Detected Overflow Exception",
	"Out of Bounds Exception",
	"Invalid Opcode Exception",
	"No Coprocessor Exception",
	"Double Fault Exception",
	"Coprocessor Segment Overrun Exception",
	"Bad TSS Exception",
	"Segment Not Present Exception",
	"Stack Fault Exception",
	"General Protection Fault Exception",
	"Page Fault Exception",
	"Unknown Interrupt Exception",
	"Coprocessor Fault Exception",
	"Alignment Check Exception",
	"Machine Check Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
};

void fault_handler(regs_t * regs)
{
	if (regs->int_no < 32) {
		panic("%s\n", exception_messages[regs->int_no]);
	}
}

/* This array is actually an array of function pointers. We use
 *  this to handle custom IRQ handlers for a given IRQ */
void * irq_routines[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

typedef void (* irq_handler_t)(regs_t * regs);

void irq_handler_install(int irq, irq_handler_t handler)
{
	irq_routines[irq] = handler;
}

void irq_handler_uninstall(int irq)
{
	irq_routines[irq] = 0;
}

void irq_handler(regs_t * regs)
{
	irq_handler_t handler;

	/* Find out if we have a custom handler to run for this
	 *  IRQ, and then finally, run it */
	handler = irq_routines[regs->int_no - 32];
	if (handler) {
		handler(regs);
	}

	/* If the IDT entry that was invoked was greater than 40
	 *  (meaning IRQ8 - 15), then we need to send an EOI to
	 *  the slave controller */
	if (regs->int_no >= 40) {
		port_out8(0x20, 0xA0);
	}

	/* In either case, we need to send an EOI to the master
	 *  interrupt controller too */
	port_out8(0x20, 0x20);
}

extern void isr_00(void);
extern void isr_01(void);
extern void isr_02(void);
extern void isr_03(void);
extern void isr_04(void);
extern void isr_05(void);
extern void isr_06(void);
extern void isr_07(void);
extern void isr_08(void);
extern void isr_09(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);

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

int idt_init(void)
{
	int i;

	/* Clear all gates */
	for (i = 0; i < IDT_ENTRIES; i++) {
		idt_gate_clear(i);
	}

	idt_interrupt_set(0,  isr_00);
	idt_interrupt_set(1,  isr_01);
	idt_interrupt_set(2,  isr_02);
	idt_interrupt_set(3,  isr_03);
	idt_interrupt_set(4,  isr_04);
	idt_interrupt_set(5,  isr_05);
	idt_interrupt_set(6,  isr_06);
	idt_interrupt_set(7,  isr_07);
	idt_interrupt_set(8,  isr_08);
	idt_interrupt_set(9,  isr_09);
	idt_interrupt_set(10, isr_10);
	idt_interrupt_set(11, isr_11);
	idt_interrupt_set(12, isr_12);
	idt_interrupt_set(13, isr_13);
	idt_interrupt_set(14, isr_14);
	idt_interrupt_set(15, isr_15);
	idt_interrupt_set(16, isr_16);
	idt_interrupt_set(17, isr_17);
	idt_interrupt_set(18, isr_18);
	idt_interrupt_set(19, isr_19);
	idt_interrupt_set(20, isr_20);
	idt_interrupt_set(21, isr_21);
	idt_interrupt_set(22, isr_22);
	idt_interrupt_set(23, isr_23);
	idt_interrupt_set(24, isr_24);
	idt_interrupt_set(25, isr_25);
	idt_interrupt_set(26, isr_26);
	idt_interrupt_set(27, isr_27);
	idt_interrupt_set(28, isr_28);
	idt_interrupt_set(39, isr_29);
	idt_interrupt_set(30, isr_30);
	idt_interrupt_set(31, isr_31);

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
