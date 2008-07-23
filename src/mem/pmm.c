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
#include "libcompiler/cdefs.h"
#include "libc/stdint.h"
#include "libc/string.h"
#include "libc/stdlib.h"
#include "libc/stddef.h"
#include "libs/debug.h"
#include "mem/pmm.h"
#include "archs/boot/bootinfo.h"
#include "dbg/debugger.h"

#define BANNER          "pmm: "

#if CONFIG_PMM_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

typedef struct {
	paddr_t address;
} pmm_entry_t;

int pmm_init(void)
{
	return 1;
}

void pmm_fini(void)
{
}

paddr_t pmm_page_alloc(pmm_page_type_t type)
{
	unused_argument(type);

	return 0;
}

int pmm_page_free(paddr_t address)
{
	unused_argument(address);

	return 0;
}

int pmm_page_use_inc(paddr_t address)
{
	unused_argument(address);

	return 0;
}

int pmm_page_use_dec(paddr_t address)
{
	unused_argument(address);

	return 0;
}
