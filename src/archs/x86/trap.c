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
#include "archs/x86/idt.h"
#include "dbg/debug.h"

#if CONFIG_TRAP_DEBUG
#define dprintf(F,A...) printf("trap: " F,##A)
#else
#define dprintf(F,A...)
#endif

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
        "Reserved Exception"
};

/* Lame but ... better than hardwiring a constant value */
#define KNOWN_EXCEPTIONS (sizeof(exception_messages) / sizeof(char *))

void trap_handler(regs_t * regs)
{
        assert(regs);

#if CONFIG_TRAP_DUMP
        idt_frame_dump(regs);
#endif
        if (regs->isr_no < KNOWN_EXCEPTIONS) {
                panic("%s", exception_messages[regs->isr_no]);
        }

        panic("Unknown trap no %d", regs->isr_no);
}
