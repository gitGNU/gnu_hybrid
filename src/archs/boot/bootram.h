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

#ifndef BOOTRAM_H
#define BOOTRAM_H

#include "config/config.h"
#include "mem/address.h"

struct bmap {
	size_t    size; /* Size in bits */
	uint8_t * data; /* Buffer */
};
typedef struct bmap bmap_t;

int    bootram_init(void);
void   bootram_fini(void);

int    bootram_reserve(addr_t start,
		       addr_t stop);
int    bootram_unreserve(addr_t start,
			 addr_t stop);

addr_t bootram_alloc(size_t size);
void   bootram_free(addr_t start,
		    size_t size);

#endif /* BOOTRAM_H */
