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

#ifndef IDT_H
#define IDT_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

/* This struct defines the stack after an ISR (TRAP/IRQ) was running */
struct regs {
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
};
typedef struct regs regs_t;

int  idt_init(void);
void idt_fini(void);
void idt_frame_dump(regs_t * regs);

__END_DECLS

#endif /* IDT_H */
