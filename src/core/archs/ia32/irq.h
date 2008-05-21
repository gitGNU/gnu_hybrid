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

#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include "config/config.h"
#include "libc/stddef.h"
#include "core/arch/idt.h"

typedef void (* irq_handler_t)(regs_t * regs);

int  irq_init(void);
void irq_fini(void);
void irq_handler_install(uint_t        irq,
			 irq_handler_t handler);
void irq_handler_uninstall(uint_t irq);
void irq_handler(regs_t * regs);


#endif /* ARCH_IRQ_H */
