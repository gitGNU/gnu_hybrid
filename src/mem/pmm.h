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

__BEGIN_DECLS

int    pmm_init(bootinfo_t * bi);
void   pmm_fini(void);

#define PMM_FLAG_VALID   0x01
#define PMM_FLAG_USED    0x02
#define PMM_FLAG_ENABLED 0x04
#define PMM_FLAG_ALL     0xFF

typedef uint8_t pmm_type_t;
int    pmm_foreach(pmm_type_t flags,
		   int        (* callback)(uint_t     start,
					   uint_t     stop,
					   pmm_type_t flags));
uint_t pmm_reserve(uint_t start,
		   uint_t stop);
void   pmm_release(uint_t start);

uint_t pmm_alloc(size_t size);
void   pmm_free(uint_t start);

uint_t pmm_page_alloc(size_t size);

__END_DECLS

#endif /* CORE_MEM_PMM_H */
