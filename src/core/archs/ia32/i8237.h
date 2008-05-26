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

#ifndef I8237_H
#define I8237_H

#include "config/config.h"
#include "core/mem/address.h"
#include "libc/stdint.h"

__BEGIN_DECLS

int    i8237_init(void);
int    i8237_fini(void);

int    i8237_start(uint_t chan,
		   addr_t addr,
		   size_t count,
		   int    read);

int    i8237_stop(uint_t chan);
size_t i8327_channels(void);

__END_DECLS

#endif /* I8237_H */
