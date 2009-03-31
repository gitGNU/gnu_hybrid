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

#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include "config/config.h"
#include "libc/stddef.h"
#if ARCH_X86
#include "archs/x86/arch.h"
#include "archs/x86/idt.h"
#endif

int               irq_init(void);
void              irq_fini(void);
void              irq_enable(void);
void              irq_disable(void);
void              irq_unmask(uint_t irq);
void              irq_mask(uint_t irq);
arch_irqs_state_t irq_state_get(void);
void              irq_state_set(arch_irqs_state_t * state);

#endif /* ARCH_IRQ_H */
