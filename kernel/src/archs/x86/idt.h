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

#ifndef IDT_H
#define IDT_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

/* This struct defines the stack after an ISR (TRAP/IRQ) was running */
struct regs {
#if 0
	/* Software frame */
	uint_t ebx;
	uint_t ecx;
	uint_t edx;
	uint_t esi;
	uint_t edi;
	uint_t ebp;
	uint_t eax;
	uint_t ds;
	uint_t es;

	/* Hardware frame */
	uint_t isr_no;
	uint_t err_code;
	uint_t eip;
	uint_t cs;
	uint_t eflags;
	uint_t esp;
	uint_t ss;
#endif
	/* Data segment selector */
	uint32_t ds;
	/* Pushed by pusha */
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	/* Interrupt number and error code (when applicable) */
	uint32_t isr_no, err_code;
	/* Pushed by the processor automatically */
	uint32_t eip, cs, eflags, user_esp, ss;
};
typedef struct regs regs_t;

#define I8259_IDT_BASE_INDEX 0x20 /* 32 */

int  idt_init(void);
void idt_clear(void);
void idt_fini(void);
void idt_frame_dump(regs_t * regs);

__END_DECLS

#endif /* IDT_H */
