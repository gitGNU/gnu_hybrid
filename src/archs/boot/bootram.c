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
#include "libc/stddef.h"
#include "libc/stdint.h"
#include "libc/string.h"
#include "libs/debug.h"
#include "archs/boot/bootram.h"

#define BANNER          "bootram: "

#if CONFIG_BOOTRAM_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

/* XXX FIXME: Ugly */
#define UINT8_BITS      (sizeof(uint8_t) * 8)

#define BMAP_CHECK(B) {				\
	assert((B));				\
	assert((B)->size);			\
	assert((B)->data);			\
}

static int bmap_bit_test_set(bmap_t * bmap,
			     size_t   index)
{
	size_t q;
	size_t r;

	BMAP_CHECK(bmap);
	assert(bmap->size > index);

	q = index / UINT8_BITS;
	r = index % UINT8_BITS;

	return (bmap->data[q] & (~(1 << r)));
}

static int bmap_bit_test_reset(bmap_t * bmap,
			       size_t   index)
{
	return !bmap_bit_test_set(bmap, index);
}

static void bmap_bit_set(bmap_t * bmap,
			 size_t   index)
{
	size_t q;
	size_t r;

	BMAP_CHECK(bmap);
	assert(bmap->size > index);

	q = index / UINT8_BITS;
	r = index % UINT8_BITS;

	bmap->data[q] |= (1 << r);
}

static void bmap_bit_reset(bmap_t * bmap,
			   size_t   index)
{
	size_t q;
	size_t r;

	BMAP_CHECK(bmap);
	assert(bmap->size > index);

	q = index / UINT8_BITS;
	r = index % UINT8_BITS;

	bmap->data[q] &= ~(1 << r);
}

#if CONFIG_BOOTRAM_DEBUG
static size_t bmap_bit_accumulate(bmap_t * bmap,
				  size_t   start,
				  size_t   stop,
				  int      (* op)(bmap_t * bmap,
						  size_t   i))
{
	size_t i;
	size_t c;

	BMAP_CHECK(bmap);
	assert(op);

	i = 0;
	c = 0;
	for (i = start; i <= stop; i++) {
		c += op(bmap, i);
	}

	return c;
}

static size_t bmap_count_set(bmap_t * bmap)
{
	assert(bmap);

	return bmap_bit_accumulate(bmap,
				   0, bmap->size - 1,
				   bmap_bit_test_set);
}

static size_t bmap_count_reset(bmap_t * bmap)
{
	assert(bmap);

	return bmap_bit_accumulate(bmap,
				   0, bmap->size - 1,
				   bmap_bit_test_reset);
}
#endif

#if 0
static int bmap_init(bmap_t *  bmap,
		     size_t    size,
		     uint8_t * data)
{
	assert(bmap);

	bmap->size = size;
	bmap->data = data;

	memset(data, 0, size / UINT8_BITS);

	BMAP_CHECK(bmap);

#if CONFIG_BOOTRAM_DEBUG
	assert(bmap_count_reset(bmap) == size);
#endif

	return 1;
}

static void bmap_fini(bmap_t * bmap)
{
	BMAP_CHECK(bmap);

	bmap->size = 0;
	bmap->data = 0;
}
#endif

struct bootram_node {
	addr_t                start; /* Start address */
	bmap_t *              bmap;  /* Pointer to bmap */
	struct bootram_node * next;  /* Next node in list */
};
typedef struct bootram_node bootram_node_t;

static bootram_node_t * head;

#if CONFIG_BOOTRAM_DEBUG
size_t bootram_count_reserved(void)
{
	size_t           count;
	bootram_node_t * tmp;

	count = 0;
	for (tmp = head; tmp; tmp = tmp->next) {
		BMAP_CHECK(tmp->bmap);

		count += bmap_count_reset(tmp->bmap);
	}

	return count;
}

size_t bootram_count_unreserved(void)
{
	size_t           count;
	bootram_node_t * tmp;

	count = 0;
	for (tmp = head; tmp; tmp = tmp->next) {
		BMAP_CHECK(tmp->bmap);

		count += bmap_count_set(tmp->bmap);
	}

	return count;
}

size_t bootram_count_all(void)
{
	size_t           count;
	bootram_node_t * tmp;

	count = 0;

	for (tmp = head; tmp; tmp = tmp->next) {
		BMAP_CHECK(tmp->bmap);

		count += tmp->bmap->size;
	}

	assert(count == bootram_count_reserved() + bootram_count_unreserved());

	return count;
}
#endif

int bootram_init(void)
{
	head = NULL;

	dprintf("Initializing bootram\n");

	return 1;
}

void bootram_fini(void)
{
#if CONFIG_BOOTRAM_DEBUG
	dprintf("Claimed back %d/%d/%d pages\n",
		bootram_count_reserved(),
		bootram_count_unreserved(),
		bootram_count_all());
#endif

	head = NULL;
}

int bootram_node_add(addr_t start,
		     size_t size)
{
	assert(size);

	dprintf("Adding bootram node\n");

	dprintf("Node spans addresses 0x%p-0x%p\n",
		start, start + size - 1);

	return 1;
}

static bootram_node_t * bootram_node_find(addr_t start,
					  size_t size)
{
	bootram_node_t * tmp;

	tmp = head;
	while (tmp) {
		BMAP_CHECK(tmp->bmap);

		if ((tmp->start <= start) &&
		    (tmp->start + tmp->bmap->size * CONFIG_PAGE_SIZE >=
		     start + size * CONFIG_PAGE_SIZE)) {
			/* Got it */
			return tmp;
		}

		tmp = tmp->next;
	}

	return tmp;
}

static void bmap_iterate(bmap_t * bmap,
			 size_t   start,
			 size_t   stop,
			 void     (* op)(bmap_t * bmap,
					 size_t   i))
{
	size_t i;

	BMAP_CHECK(bmap);
	assert(op);

	i = 0;
	for (i = start; i <= stop; i++) {
		op(bmap, i);
	}
}

int bootram_reserve(addr_t start,
		    size_t size)
{
	bootram_node_t * n;

	n = bootram_node_find(start, size);
	if (!n) {
		dprintf("Cannot find a node for "
			"reserving addresses 0x%p-0x%p\n",
			start, start + size - 1);
		return 0;
	}

	assert(n->start <= start);

	bmap_iterate(n->bmap,
		     (n->start - start) /
		     CONFIG_PAGE_SIZE,
		     (n->start - start + size * CONFIG_PAGE_SIZE) /
		     CONFIG_PAGE_SIZE,
		     bmap_bit_reset);

	return 1;
}

int bootram_unreserve(addr_t start,
		      size_t size)
{
	bootram_node_t * n;

	n = bootram_node_find(start, size);
	if (!n) {
		dprintf("Cannot find node for "
			"unreserving addresses 0x%p-0x%p\n",
			start, start + size - 1);

		return 0;
	}

	assert(n->start <= start);

	bmap_iterate(n->bmap,
		     (n->start - start) /
		     CONFIG_PAGE_SIZE,
		     ((n->start - start) + size * CONFIG_PAGE_SIZE) /
		     CONFIG_PAGE_SIZE,
		     bmap_bit_set);

	return 1;
}

addr_t bootram_alloc(size_t size)
{
	unused_argument(size);

	missing();

	return 0;
}

void bootram_free(addr_t start,
		  size_t size)
{
	unused_argument(start);
	unused_argument(size);

	missing();
}
