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

#ifndef HASH_H
#define HASH_H

#include "config/config.h"

__BEGIN_DECLS

typedef struct {
	uint_t      hash;
	const char* key;
	void*       value;
} hash_record_t;

typedef struct {
	hash_record_t* records;
	uint_t         records_count;
	size_t         size_index;
} hash_t;

hash_t* hash_new(size_t size);
void    hash_delete(hash_t* hash);

int     hash_add(hash_t*     hash,
		 const char* key,
		 void*       value);
void*   hash_get(hash_t*     hash,
		 const char* key);
void    hash_remove(hash_t*     hash,
		    const char* key);
size_t  hash_size(hash_t* hash);

__END_DECLS

#endif /* HASH_H */
