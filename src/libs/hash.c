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
#include "libs/hash.h"
#include "libc/stdlib.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "libm/math.h"
#include "core/dbg/debug.h"

/*
 * NOTE:
 *     Table is sized by primes to minimize clustering. For more informations
 *     see the following address:
 *
 *         http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
 */
static const uint_t sizes[] = {
	53,
	97, 
	193,
	389,
	769,
	1543,
	3079,
	6151,
	12289,
	24593,
	49157,
	98317,
	196613,
	393241,
	786433,
	1572869,
	3145739,
	6291469,
	12582917,
	25165843,
	50331653,
	100663319,
	201326611,
	402653189,
	805306457,
	1610612741
};

#define HASH_SIZES_COUNT ((size_t) (sizeof(sizes) / sizeof(sizes[0])))
#define HASH_LOAD_FACTOR ((float) 0.65)

static int hash_grow(hash_t* hash)
{
	size_t         i;
	hash_record_t* old_recs;
	size_t         old_recs_length;

	assert(hash);

	old_recs_length = sizes[hash->size_index];
	old_recs        = hash->records;

	if (hash->size_index == HASH_SIZES_COUNT - 1) {
		return 0;
	}
	if ((hash->records = calloc(sizes[++hash->size_index],
				    sizeof(hash_record_t))) == NULL) {
		hash->records = old_recs;
		return 0;
	}

	hash->records_count = 0;

	/* Rehash table */
	for (i = 0; i < old_recs_length; i++) {
		if (old_recs[i].hash && old_recs[i].key) {
			int retval;
			
			retval = hash_add(hash,
					  old_recs[i].key,
					  old_recs[i].value);

			assert(retval);
		}
	}

	free(old_recs);

	return 1;
}

static uint_t strhash(const char* str)
{
	int c;
	int hash;

	hash = 5381;
	while ((c = *str++) != 0) {
		hash = hash * 33 + c;
	}

	return ((hash == 0) ? 1 : hash);
}

hash_t* hash_new(size_t size)
{
	hash_t* hash;
	size_t  i;
	int     sind;

	hash = malloc(sizeof(hash_t));
	if (!hash) {
		return NULL;
	}

	size /= HASH_LOAD_FACTOR;

	sind = 0;
	for (i = 0; i < HASH_SIZES_COUNT; i++) {
		if (sizes[i] > size) {
			sind = i;
			break;
		}
	}

	hash->records = calloc(sizes[sind], sizeof(hash_record_t));
	if (!hash->records) {
		free(hash);
		return NULL;
	}

	hash->records_count = 0;
	hash->size_index    = sind;

	return hash;
}

void hash_delete(hash_t* hash)
{
	free(hash->records);
	free(hash);
}

int hash_add(hash_t*     hash,
	     const char* key,
	     void*       value)
{
	hash_record_t* recs;
	int            rc;
	uint_t         off, ind, size, code;

	if (key == NULL || *key == '\0') {
		return -2;
	}
	if (hash->records_count > sizes[hash->size_index] * HASH_LOAD_FACTOR) {
		rc = hash_grow(hash);
		if (!rc) {
			return 0;
		}
	}

	code = strhash(key);
	recs = hash->records;
	size = sizes[hash->size_index];

	ind = code % size;
	off = 0;

	while (recs[ind].key) {
		off++;
		ind = (code + ((int) pow(off, 2))) % size;
	}

	recs[ind].hash  = code;
	recs[ind].key   = key;
	recs[ind].value = value;

	hash->records_count++;

	return 1;
}

void*  hash_get(hash_t*     hash,
		const char* key)
{
	hash_record_t* recs;
	uint_t         off, ind, size;
	uint_t         code;

	code = strhash(key);
	recs = hash->records;
	size = sizes[hash->size_index];
	ind  = code % size;
	off  = 0;

	while (recs[ind].hash) {
		if ((code == recs[ind].hash)   &&
		    recs[ind].key              &&
		    strcmp(key, recs[ind].key) == 0) {
			return recs[ind].value;
		}
		off++;
		ind = (code + ((int) pow(off, 2))) % size;
	}

	return NULL;
}

void hash_remove(hash_t*     hash,
		 const char* key)
{
	uint_t         code;
	hash_record_t* recs;
	uint_t         off;
	uint_t         ind;
	uint_t         size;

	code = strhash(key);
	recs = hash->records;
	size = sizes[hash->size_index];
	ind  = code % size;
	off  = 0;

	while (recs[ind].hash) {
		if ((code == recs[ind].hash)   &&
		    recs[ind].key              &&
		    strcmp(key, recs[ind].key) == 0) {
			recs[ind].key   = 0;
			recs[ind].value = 0;
			hash->records_count--;
			return;
		}
		off++;
		ind = (code + ((int) pow(off, 2))) % size;
	}
 
	return;
}

size_t hash_size(hash_t* hash)
{
	assert(hash);

	return hash->records_count;
}
