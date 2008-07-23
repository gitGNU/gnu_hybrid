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
#define UINT8_DIV_SHIFT 3
#define UINT8_DIV_MASK  0x07

struct bmap {
	size_t    size; /* Size in bits */
	uint8_t * data; /* Buffer */
};
typedef struct bmap bmap_t;

#define BMAP_CHECK(B) {				\
	assert((B));				\
	assert((B)->size);			\
	assert((B)->data);			\
}

#if CONFIG_BOOTRAM_DEBUG
#define BMAP_DUMP(B) {						\
	dprintf("bmap 0x%p stats:\n",				\
		(B));						\
	dprintf("  size %d bits (maps %u bytes)\n",		\
		(B)->size,  (B)->size * CONFIG_PAGE_SIZE);	\
	dprintf("  data 0x%p\n",				\
		(B)->data);					\
}
#else
#define BMAP_DUMP(B) { }
#endif

#if 0
static void bmap_iterate(bmap_t * bmap,
			 size_t   start,
			 size_t   stop,
			 void     (* op)(bmap_t * bmap,
					 size_t   i))
{
	size_t i;

	dprintf("Iterating over bits %d thru %d on bmap 0x%p\n",
		start, stop, bmap);

	BMAP_CHECK(bmap);
	assert(op);

	BMAP_DUMP(bmap);

	i = 0;
	for (i = start; i <= stop; i++) {
		op(bmap, i);
	}
}
#endif

static int bmap_test_set(bmap_t * bmap,
			 size_t   index)
{
	size_t q;
	size_t r;

	BMAP_CHECK(bmap);
	assert(bmap->size > index);

	q = index >> UINT8_DIV_SHIFT;
	r = index &  UINT8_DIV_MASK;

	return (bmap->data[q] & (~(1 << r)));
}

static int bmap_test_reset(bmap_t * bmap,
			   size_t   index)
{
	return !bmap_test_set(bmap, index);
}

#if CONFIG_BOOTRAM_DEBUG
static size_t bmap_accumulate(bmap_t * bmap,
			      size_t   start,
			      size_t   stop,
			      int      (* op)(bmap_t * bmap,
					      size_t   i))
{
	size_t i;
	size_t c;

	dprintf("Accumulating bits %d thru %d on bmap 0x%p\n",
		start, stop, bmap);

	BMAP_CHECK(bmap);
	assert(op);

	BMAP_DUMP(bmap);

	i = 0;
	c = 0;
	for (i = start; i <= stop; i++) {
		c += op(bmap, i);
	}

	return c;
}

static size_t bmap_count_set(bmap_t * bmap)
{
	BMAP_CHECK(bmap);

	return bmap_accumulate(bmap, 0, bmap->size - 1, bmap_test_set);
}

static size_t bmap_count_reset(bmap_t * bmap)
{
	BMAP_CHECK(bmap);

	return bmap_accumulate(bmap, 0, bmap->size - 1, bmap_test_reset);
}
#endif

/* This function shouldn't be called directly */
static void bmap_set(bmap_t * bmap,
		     size_t   index)
{
	size_t q;
	size_t r;

	/* BMAP_CHECK(bmap); */

	assert(index < bmap->size);

	q = index >> UINT8_DIV_SHIFT;
	r = index &  UINT8_DIV_MASK;

	bmap->data[q] |= (1 << r);
}

static void bmap_set_range(bmap_t * bmap,
			   size_t   start,
			   size_t   stop)
{
	size_t i;

	BMAP_CHECK(bmap);

	assert(start <= stop);
	/* assert(start >=0); */
	assert(stop < bmap->size);

	for (i = start; i <= stop; i++) {
		bmap_set(bmap, i);
	}
}

static void bmap_set_all(bmap_t * bmap)
{
	BMAP_CHECK(bmap);

	bmap_set_range(bmap, 0, bmap->size - 1);
}

/* This function shouldn't be called directly */
static void bmap_reset(bmap_t * bmap,
		       size_t   index)
{
	size_t q;
	size_t r;

	/* BMAP_CHECK(bmap); */

	assert(index < bmap->size);

	q = index >> UINT8_DIV_SHIFT;
	r = index &  UINT8_DIV_MASK;

	bmap->data[q] &= ~(1 << r);
}

static void bmap_reset_range(bmap_t * bmap,
			     size_t   start,
			     size_t   stop)
{
	size_t i;

	BMAP_CHECK(bmap);

	assert(start <= stop);
	/* assert(start >=0); */
	assert(stop < bmap->size);

	for (i = start; i <= stop; i++) {
		bmap_reset(bmap, i);
	}
}

static void bmap_reset_all(bmap_t * bmap)
{
	BMAP_CHECK(bmap);

	bmap_reset_range(bmap, 0, bmap->size - 1);
}

static int bmap_ffs(bmap_t * bmap,
		    size_t   start,
		    size_t * got)
{
	size_t i;

	BMAP_CHECK(bmap);
	assert(bmap->size > start);
	assert(got);

	dprintf("Looking for the first set bit on bmap 0x%p\n", bmap);
	dprintf("Starting from %d (size = %d)\n", bmap, start, bmap->size);

	for (i = start; i < bmap->size; i++) {
		if (bmap_test_set(bmap, i)) {
			dprintf("First free on bmap 0x%p is %d\n", bmap, i);
			*got = i;
			return 1;
		}
	}

	return 0;
}

static int bmap_init(bmap_t *  bmap,
		     size_t    size,
		     uint8_t * data)
{
	assert(bmap);

	bmap->size = size;
	bmap->data = data;

	BMAP_CHECK(bmap);

	dprintf("Initializing bmap 0x%p\n", bmap);

	/* bmap_reset_all(bmap); */

#if CONFIG_BOOTRAM_DEBUG
	assert(bmap_count_reset(bmap) == size);

	BMAP_DUMP(bmap);
#endif

	return 1;
}

static void bmap_fini(bmap_t * bmap)
{
	assert(bmap);

	bmap->size = 0;
	bmap->data = 0;
}

/*
 * Bootram bnode utilities
 */

struct bnode {
	paddr_t        start; /* Start address */
	paddr_t        stop;  /* Stop address */
	bmap_t *       bmap;  /* Pointer to page bitmap */
	struct bnode * next;  /* Next bnode in list */
};
typedef struct bnode bnode_t;

#define BNODE_CHECK(B) {			\
	assert(B);				\
	assert((B)->start < (B)->stop);		\
	assert((B)->bmap);			\
}

#if CONFIG_BOOTRAM_DEBUG
#define BNODE_DUMP(N) {						\
	dprintf("bnode 0x%p stats:\n", (N));			\
	dprintf("  start 0x%p\n", (paddr_t) (N)->start);	\
	dprintf("  stop  0x%p\n", (paddr_t) (N)->stop);		\
	dprintf("  bmap  0x%p\n", (N)->bmap);			\
	dprintf("  next  0x%p\n", (N)->next);			\
}
#else
#define BNODE_DUMP(N) { }
#endif

static int bnode_init(bnode_t *  bnode,
		      paddr_t    start,
		      paddr_t    stop,
		      bmap_t *   bmap)
{
	assert(bnode);

	BMAP_CHECK(bmap);

	bnode->start = start;
	bnode->stop  = stop;
	bnode->bmap  = bmap;
	bnode->next  = 0;

	BNODE_CHECK(bnode);

	BNODE_DUMP(bnode);

	assert(((bnode->stop - bnode->start) / CONFIG_PAGE_SIZE) ==
	       bmap->size);

	return 1;
}

static int bnode_reserve(bnode_t *  bnode,
			 paddr_t    start,
			 paddr_t    stop)
{
	BNODE_CHECK(bnode);

	assert(start < stop);
	assert(start >= bnode->start);
	assert(stop  <= bnode->stop);

	/* XXX FIXME: This is a gross hack */
	if ((start <= bnode->start) && (stop >= bnode->stop)) {
		dprintf("Reserving all pages on bnode 0x%p\n", bnode);

		bmap_reset_all(bnode->bmap);
	} else {
		size_t page_start;
		size_t page_stop;

		page_start = start / CONFIG_PAGE_SIZE;
		page_stop  = stop  / CONFIG_PAGE_SIZE;

		dprintf("Reserving page range %d-%d on bnode 0x%p\n",
			page_start, page_stop, bnode);

		bmap_reset_range(bnode->bmap, page_start, page_stop);
	}

	BNODE_DUMP(bnode);

	return 1;
}

static int bnode_unreserve(bnode_t *  bnode,
			   paddr_t    start,
			   paddr_t    stop)
{
	BNODE_CHECK(bnode);

	assert(start < stop);
	assert(start >= bnode->start);
	assert(stop  <= bnode->stop);

	/* XXX FIXME: This is a gross hack */
	if ((start <= bnode->start) && (stop >= bnode->stop)) {
		dprintf("Reserving all pages on bnode 0x%p\n", bnode);

		bmap_set_all(bnode->bmap);
	} else {
		size_t page_start;
		size_t page_stop;

		page_start = start / CONFIG_PAGE_SIZE;
		page_stop  = stop  / CONFIG_PAGE_SIZE;

		dprintf("Unreserving page range %d-%d on bnode 0x%p\n",
			page_start, page_stop, bnode);

		bmap_set_range(bnode->bmap, page_start, page_stop);
	}

	BNODE_DUMP(bnode);

	return 1;
}

static int bnode_fini(bnode_t * bnode)
{
	assert(bnode);

	bnode->start = 0;
	bnode->stop  = 0;
	bnode->bmap  = NULL;
	bnode->next  = NULL;

	return 1;
}

/*
 * APIs
 */

/* XXX FIXME: Ugly hack, please remove ASAP */
#define PAGES   (((paddr_t) - 1) / CONFIG_PAGE_SIZE)
#define ENTRIES (PAGES / UINT8_BITS)
static uint8_t   data_[ENTRIES] SECTION(".bootstrap");
static bmap_t    map_           SECTION(".bootstrap");
static bnode_t   node_          SECTION(".bootstrap");
static bnode_t * head_          SECTION(".bootstrap");

#if CONFIG_BOOTRAM_DEBUG
#define BRAM_DUMP() {						\
	bnode_t * tmp;						\
								\
	dprintf("Dump starts here\n");				\
	for (tmp = head_; tmp != NULL; tmp = tmp->next) {	\
		BNODE_DUMP(tmp);				\
	}							\
	dprintf("Dump ends here\n");				\
}
#else
#define BRAM_DUMP() { }
#endif

#if CONFIG_BOOTRAM_DEBUG
static size_t bram_count_reserved(void)
{
	size_t    count;
	bnode_t * tmp;

	dprintf("Counting reserved bits\n");

	count = 0;
	for (tmp = head_; tmp != NULL; tmp = tmp->next) {
		BNODE_CHECK(tmp);
		BMAP_CHECK(tmp->bmap);

		count += bmap_count_reset(tmp->bmap);
		dprintf("  Count is now %d (bmap 0x%p)\n", count, tmp->bmap);
	}

	dprintf("Reserved %d bits\n", count);

	return count;
}

static size_t bram_count_unreserved(void)
{
	size_t    count;
	bnode_t * tmp;

	dprintf("Counting unreserved bits\n");

	count = 0;
	for (tmp = head_; tmp != NULL; tmp = tmp->next) {
		BNODE_CHECK(tmp);
		BMAP_CHECK(tmp->bmap);

		count += bmap_count_set(tmp->bmap);
		dprintf("  Count is now %d (bmap 0x%p)\n", count, tmp->bmap);
	}

	dprintf("Unreserved %d bits\n", count);

	return count;
}

static size_t bram_count_all(void)
{
	size_t    count;
	bnode_t * tmp;

	dprintf("Counting all bits\n");

	count = 0;
	for (tmp = head_; tmp != NULL; tmp = tmp->next) {
		BNODE_CHECK(tmp);
		BMAP_CHECK(tmp->bmap);

		count += tmp->bmap->size;
		dprintf("  Count is now %d (bmap 0x%p)\n", count, tmp->bmap);
	}

	dprintf("Total %d bits\n", count);

	return count;
}
#endif

static int bram_node_add(bnode_t * bnode)
{
	bnode_t * curr;
	bnode_t * prev;

	BNODE_CHECK(bnode);

	dprintf("Adding bnode 0x%p to the pool\n", bnode);

	prev = NULL;
	curr = head_;
	while (curr && (curr->start <= bnode->start)) {
		dprintf("  prev = 0x%p, curr = 0x%p\n", prev, curr);
		prev = curr;
		curr = curr->next;
	}

	dprintf("  prev = 0x%p, curr = 0x%p\n", prev, curr);

	/* XXX FIXME: Nodes shouldn't overlap! */

	bnode->next = curr;
	if (prev) {
		prev->next = bnode;
	} else {
		head_      = bnode;
	}

	return 1;
}

static int bram_node_remove(bnode_t * bnode)
{
	bnode_t * prev, * curr;

	BNODE_CHECK(bnode);

	dprintf("Removing bnode 0x%p from the pool\n", bnode);

	prev = NULL;
	curr = head_;
	while (curr && (curr != bnode)) {
		dprintf("  prev = 0x%p, curr = 0x%p\n", prev, curr);
		prev = curr;
		curr = curr->next;
	}

	dprintf("  prev = 0x%p, curr = 0x%p\n", prev, curr);

	if (curr != bnode) {
		dprintf("Node 0x%p not found\n", bnode);
		return 0;
	}

	if (prev) {
		prev->next = curr->next;
	} else {
		head_ = curr->next;
	}

	return 1;
}

#if CONFIG_BOOTRAM_DEBUG
#define BOOTRAM_DUMP() {					\
	size_t all, reserved, unreserved;			\
								\
	all        = bram_count_all();				\
	reserved   = bram_count_reserved();			\
	unreserved = bram_count_unreserved();			\
								\
	dprintf("bootram stats:\n");				\
	dprintf("  All        %d (%u bytes)\n",			\
		all,        all * CONFIG_PAGE_SIZE);		\
	dprintf("  Reserved   %d (%u bytes)\n",			\
		reserved,   reserved * CONFIG_PAGE_SIZE);	\
	dprintf("  Unreserved %d (%u bytes)\n",			\
		unreserved, unreserved * CONFIG_PAGE_SIZE);	\
								\
	BRAM_DUMP();						\
}
#else
#define BOOTRAM_DUMP() { }
#endif

int bootram_init(void)
{
	dprintf("Initializing bootram\n");

	head_ = NULL;

	if (!bmap_init(&map_, PAGES, data_)) {
		return 0;
	}
	bmap_set_all(&map_);

	if (!bnode_init(&node_, 0, (paddr_t) -1, &map_)) {
		return 0;
	}

	if (!bram_node_add(&node_)) {
		return 0;
	}

	dprintf("Initialized\n");

	BOOTRAM_DUMP();

	return 1;
}

void bootram_fini(void)
{
	BOOTRAM_DUMP();

	bram_node_remove(&node_);
	bnode_fini(&node_);
	bmap_fini(&map_);

	head_ = NULL;
}

static int bootram_operation(paddr_t start,
			     paddr_t stop,
			     int     (* op)(bnode_t * bnode,
					    paddr_t    start,
					    paddr_t    stop))
{
	bnode_t * tmp;

	assert(op);
	assert(start <= stop);

	tmp = head_;
	while (tmp) {
		dprintf("Performing operation 0x%p on bnode 0x%p\n", op, tmp);

		if ((start <  tmp->start)  &&
		    (stop  <  tmp->stop)   &&
		    (stop  >= tmp->start)) {
			dprintf("  Operation case #1 (upper)\n");
			if (!op(tmp, tmp->start, stop)) {
				return 0;
			}
		} else if ((start >= tmp->start)  &&
			   (stop  <= tmp->stop)) {
			dprintf("  Operation case #2 (internal)\n");
			if (!op(tmp, start, stop)) {
				return 0;
			}
		} else if ((start >  tmp->start)  &&
			   (stop  >  tmp->stop)   &&
			   (start <= tmp->stop)) {
			dprintf("  Operation case #3 (lower)\n");
			if (!op(tmp, start, tmp->stop)) {
				return 0;
			}
		} else {
			bug();
		}

		tmp = tmp->next;
	}

	return 1;
}

int bootram_reserve(paddr_t start,
		    paddr_t stop)
{
	dprintf("Reserving address range 0x%p-0x%p\n", start, stop);

	return bootram_operation(start, stop, bnode_reserve);
}

int bootram_unreserve(paddr_t start,
		      paddr_t stop)
{
	dprintf("Unreserving address range 0x%p-0x%p\n", start, stop);

	return bootram_operation(start, stop, bnode_unreserve);
}

paddr_t bootram_alloc(size_t size)
{
	size_t    pages;
	bnode_t * tmp;

	pages = (size / CONFIG_PAGE_SIZE) + (size % CONFIG_PAGE_SIZE ? 1 : 0);

	dprintf("Allocating %d bytes (%d pages)\n", size, pages);

	for (tmp = head_; tmp != NULL; tmp = tmp->next) {
		size_t start;
		size_t stop;

		dprintf("Looking for %d pages on node 0x%p\n", pages, tmp);

		BNODE_DUMP(tmp);

		dprintf("Working on bmap 0x%p\n", tmp->bmap);

		BMAP_DUMP(tmp->bmap);

		if (tmp->bmap->size < size) {
			dprintf("  Node 0x%p bmap is too small (%d pages)\n",
				tmp, tmp->bmap->size);
			continue;
		}

		start = 0;
		while (start + pages - 1 <= tmp->bmap->size) {
			size_t i;

			if (!bmap_ffs(tmp->bmap, start, &start)) {
				dprintf("No free bits on bmap 0x%p\n",
					tmp->bmap);
				break;
			}

			stop = start + pages - 1;
			if (stop > tmp->bmap->size) {
				dprintf("  Node 0x%p has no enough "
					"free pages\n", tmp);
				break;
			}

			dprintf("  Exploring %d-%d\n", start, stop);

			for (i = start; i <= stop; i++) {
				if (bmap_test_reset(tmp->bmap, i)) {
					dprintf("    Page %i is used\n", i);
					break;
				}
			}
			if (i != stop + 1) {
				start = stop + 1;
				continue;
			}

			dprintf("  Pages %d-%d on node 0x%p are good\n",
				start, stop, tmp);
			for (i = start; i <= stop; i++) {
				bmap_reset(tmp->bmap, i);
			}

			return start;
		}
		dprintf("  No good pages on node 0x%p\n", tmp);
	}

	return 0;
}
