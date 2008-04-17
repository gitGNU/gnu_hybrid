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

#ifndef CORE_INTERRUPT_H
#define CORE_INTERRUPT_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

#define NR_IRQS 256

typedef void (*irq_handler_t)(void);

int irq_init(void);

int  irq_enabled(uint_t index);
int  irq_disable(uint_t index);
int  irq_enable(uint_t index);
int  irq_attach(uint_t        index,
		irq_handler_t handler);
int  irq_detach(uint_t index);
void  irqs_disable(void);
void  irqs_enable(void);
void  irqs_save(irq_flags_t* flags);
void  irqs_restore(const irq_flags_t* flags);

void  irq_fini(void);

__END_DECLS

#endif /* CORE_INTERRUPT_H */
