/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
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
#include "libcompiler/cdefs.h"
#include "libc/stdint.h"
#include "libc/string.h"
#include "libc/stdlib.h"
#include "libc/stddef.h"
#include "libs/debug.h"
#include "mem/pmm.h"
#include "archs/boot/bootram.h"
#include "dbg/debugger.h"

#define BANNER          "pmm: "

#if CONFIG_PMM_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

struct pmm_entry {
	paddr_t            address;
	struct pmm_entry * next;
};
typedef struct pmm_entry pmm_entry_t;

struct {
	pmm_entry_t * free;
} head;

#if 0
static int pmm_bootram_iterator(paddr_t start)
{
	pmm_entry_t * tmp;

	/* dprintf("Adding page 0x%p to the free list\n", start); */

	tmp = (pmm_entry_t *) start;
	tmp->address = start;
	tmp->next    = head.free;
	head.free    = tmp;

	return 1;
}
#endif

int pmm_init(void)
{
	dprintf("Initalizing\n");

	head.free = NULL;

#if 0
	if (!bootram_foreach(pmm_bootram_iterator)) {
		return 0;
	}
#endif

	return 1;
}

void pmm_fini(void)
{
}

paddr_t pmm_page_alloc(void)
{
	pmm_entry_t * t;

	t = head.free;
	if (!t) {
		panic("No page to alloc");
	}

	head.free = head.free->next;

	return t->address;
}

int pmm_page_free(paddr_t address)
{
	pmm_entry_t * t;

	unused_argument(address);

	t          = (pmm_entry_t *) address;
	t->address = address;
	t->next    = head.free;
	head.free  = t;

	return 0;
}
