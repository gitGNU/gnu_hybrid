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

#include "config/config.h"
#include "libc/stdlib.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "libs/bitmap.h"
#include "core/dbg/debug.h"

static int bitmap_init(bitmap_t* bitmap,
		       size_t    size)
{
	uint_t bytes;

	assert(bitmap);
	assert(!bitmap->buffer);

	bytes = (size / (sizeof(bitmap_chunk_t) * 8)) + 1;

	bitmap->buffer = (bitmap_chunk_t *) malloc(bytes);
	if (!bitmap->buffer) {
		return 0;
	}
	
	bitmap->size = size;

	bitmap_clear(bitmap);

	return 1;
}

static void bitmap_fini(bitmap_t* bitmap)
{
	assert(bitmap);
	assert(bitmap->buffer);

	free(bitmap->buffer);
}

bitmap_t* bitmap_new(size_t size)
{
	bitmap_t* tmp;

	tmp = malloc(sizeof(bitmap_t));
	if (!tmp) {
		return NULL;
	}

	if (!bitmap_init(tmp, size)) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

void bitmap_delete(bitmap_t* bitmap)
{
	assert(bitmap);
	assert(bitmap->buffer);

	bitmap_fini(bitmap);
	free(bitmap);
}

void bitmap_clear(bitmap_t* bitmap)
{
	assert(bitmap);
	assert(bitmap->buffer);

	memset(bitmap->buffer,
	       0,
	       (bitmap->size / (sizeof(bitmap_chunk_t) * 8)) + 1);
}

int bitmap_get(bitmap_t* bitmap,
	       size_t    index)
{
	uint_t         pos;
	uint_t         bit;
	bitmap_chunk_t mask;

	assert(bitmap);
	assert(bitmap->buffer);
	assert(index < bitmap->size);

	pos = index / (sizeof(bitmap_chunk_t) * 8);
	bit = index % (sizeof(bitmap_chunk_t) * 8);

	mask = 1 << bit;

	return ((bitmap->buffer[pos] & mask) ? 1 : 0);
}

void bitmap_set(bitmap_t* bitmap,
		size_t    index)
{
	uint_t         pos;
	uint_t         bit;
	bitmap_chunk_t mask;

	assert(bitmap);
	assert(bitmap->buffer);
	assert(index < bitmap->size);

	pos = index / (sizeof(bitmap_chunk_t) * 8);
	bit = index % (sizeof(bitmap_chunk_t) * 8);

	mask = 1 << bit;
	bitmap->buffer[pos] = bitmap->buffer[pos] | mask;
}

void bitmap_reset(bitmap_t* bitmap,
	          size_t    index)
{
	uint_t         pos;
	uint_t         bit;
	bitmap_chunk_t mask;

	assert(bitmap);
	assert(bitmap->buffer);
	assert(index < bitmap->size);

	pos = index / (sizeof(bitmap_chunk_t) * 8);
	bit = index % (sizeof(bitmap_chunk_t) * 8);

	mask = ~(1 << bit);
	bitmap->buffer[pos] = bitmap->buffer[pos] & mask;
}
