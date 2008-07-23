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

#ifndef CORE_MEM_PMM_H
#define CORE_MEM_PMM_H

#include "config/config.h"
#include "archs/boot/bootinfo.h"
#include "libc/stdint.h"
#include "mem/address.h"

int    pmm_init(void);
void   pmm_fini(void);

/* Page types */
typedef enum {
	PMM_PAGE_FREE,
	PMM_PAGE_KERNEL,
	PMM_PAGE_USER
} pmm_page_type_t;

paddr_t pmm_page_alloc(pmm_page_type_t type);
int     pmm_page_free(paddr_t address);

int     pmm_page_use_inc(paddr_t address);
int     pmm_page_use_dec(paddr_t address);
int     pmm_page_use_set(paddr_t address);
int     pmm_page_use_get(paddr_t address);

#endif /* CORE_MEM_PMM_H */
