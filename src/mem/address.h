/* -*- c -*- */

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

#ifndef CORE_MEM_ADDRESS_H
#define CORE_MEM_ADDRESS_H

#include "config/config.h"

__BEGIN_DECLS

typedef uint_t paddr_t; /* Physical address */
typedef uint_t vaddr_t; /* Virtual address */

#define virt_to_phys(X) (X)
#define phys_to_virt(X) (X)

int valid_text_address(paddr_t addr);
int valid_data_address(paddr_t addr);
int valid_rodata_address(paddr_t addr);
int valid_bss_address(paddr_t addr);
int valid_debug_address(paddr_t addr);

__END_DECLS

#endif // CORE_MEM_ADDRESS_H
