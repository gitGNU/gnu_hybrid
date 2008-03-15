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

#ifndef BITMAP_H
#define BITMAP_H

#include "config/config.h"

__BEGIN_DECLS

typedef uint8_t bitmap_chunk_t;

typedef struct {
	size_t          size;
	bitmap_chunk_t* buffer;
} bitmap_t;

bitmap_t* bitmap_new(size_t size);
void      bitmap_delete(bitmap_t* bitmap);

void      bitmap_clear(bitmap_t* bitmap);
int       bitmap_get(bitmap_t* bitmap,
		     size_t    index);
void      bitmap_set(bitmap_t* bitmap,
		     size_t    index);
void      bitmap_reset(bitmap_t* bitmap,
		       size_t    index);

__END_DECLS

#endif /* HASH_H */
