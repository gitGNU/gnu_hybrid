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
#include "mem/address.h"
#include "libc/stdint.h"
#include "libc/string.h"
#include "libs/debug.h"

#define BANNER          "bootmem: "

#if CONFIG_BOOTMEM_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

/* XXX FIXME: Ugly */
#define UINT8_BITS    (sizeof(uint8_t) * 8)
/* XXX FIXME: Ugly */
#define BOOTMEM_PAGES (((addr_t) -1) / CONFIG_PAGE_SIZE)
/* XXX FIXME: Ugly */
#define BOOTMEM_BYTES (BOOTMEM_PAGES / UINT8_BITS)

typedef uint8_t bootmem_bitset_t[BOOTMEM_BYTES];

struct bootmem_node {
	int              initialized;
	bootmem_bitset_t bitset;
	/* bootmem_node *   next; */
};
typedef struct bootmem_node bootmem_node_t;

bootmem_node_t bootmem  SECTION(".bootmem");

int bootmem_init(void)
{
	assert(!bootmem.initialized);

	bootmem.initialized = 1;
	memset(bootmem.bitset, 0, sizeof(bootmem.bitset));

	dprintf("Bootmem memory spans 0x%p-0x%p\n",
		0, (BOOTMEM_PAGES + 1) * CONFIG_PAGE_SIZE - 1);

	dprintf("Bootmem initialized for %d pages (bitset is %d bytes)\n",
		BOOTMEM_PAGES, sizeof(bootmem_bitset_t));

	return 1;
}

void bootmem_fini(void)
{
	assert(bootmem.initialized);

	bootmem.initialized = 1;

	dprintf("Claimed back %d bytes\n", sizeof(bootmem_bitset_t));
}

#if CONFIG_BOOTMEM_DEBUG
static size_t bootmem_count_unreserved(void)
{
	size_t count;
	size_t i;

	count = 0;
	for (i = 0; i < BOOTMEM_BYTES; i++) {
		uint8_t j;

		j = bootmem.bitset[i];
		while (j) {
			count += j & 1;
			j >>= 1;
		}
	}

	return count;
}
#endif

int bootmem_reserve(addr_t start,
		    addr_t stop)
{
	addr_t pg_start;
	addr_t pg_stop;
	size_t i;

	assert(bootmem.initialized);
	assert(start < stop);

	pg_start = start / CONFIG_PAGE_SIZE;
	pg_stop  = stop  / CONFIG_PAGE_SIZE;

	assert(pg_start <= pg_stop);

	dprintf("Reserving pages rage [%d-%d] (%d)\n",
		pg_start, pg_stop,  pg_stop - pg_start + 1);

	for (i = pg_start; i <= pg_stop; i++) {
		size_t j;

		j = i / UINT8_BITS;
		bootmem.bitset[j] =
			bootmem.bitset[j] & (!(1 << (i % UINT8_BITS)));
	}

#if CONFIG_BOOTMEM_DEBUG
	dprintf("Pages %d/%d\n", bootmem_count_unreserved(), BOOTMEM_PAGES);
#endif

	return 1;
}

int bootmem_unreserve(addr_t start,
		      addr_t stop)
{
	addr_t pg_start;
	addr_t pg_stop;
	size_t i;

	assert(bootmem.initialized);
	assert(start < stop);

	pg_start = start / CONFIG_PAGE_SIZE;
	pg_stop  = stop  / CONFIG_PAGE_SIZE;

	assert(pg_start <= pg_stop);

	dprintf("Unreserving pages range [%d-%d] (%d)\n",
		pg_start, pg_stop, pg_stop - pg_start + 1);

	for (i = pg_start; i <= pg_stop; i++) {
		size_t j;

		j = i / UINT8_BITS;
		bootmem.bitset[j] =
			bootmem.bitset[j] | (1 << (i % UINT8_BITS));
	}

#if CONFIG_BOOTMEM_DEBUG
	dprintf("Pages %d/%d\n", bootmem_count_unreserved(), BOOTMEM_PAGES);
#endif

	return 1;
}
