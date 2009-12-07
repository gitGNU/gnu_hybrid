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

#ifndef I8259_H
#define I8259_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

#define I8259_IRQS 16

typedef uint16_t i8259_mask_t;

int          i8259_init(void);
void         i8259_fini(void);
i8259_mask_t i8259_mask_get(void);
void         i8259_mask_set(i8259_mask_t mask);
void         i8259_enable(uint_t irq);
void         i8259_disable(uint_t irq);
void         i8259_eoi(uint_t irq);

__END_DECLS

#endif /* I8259_H */
