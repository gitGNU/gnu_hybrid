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

#ifndef ARCHS_ARCH_H
#define ARCHS_ARCH_H

#include "config.h"
#include "libc/stdint.h"

void   arch_halt(void);
void   arch_power_off(void);
void   arch_reset(void);
uint_t arch_backtrace_store(uint_t * backtrace,
			    uint_t   max_len);

#endif /* ARCHS_ARCH_H */
