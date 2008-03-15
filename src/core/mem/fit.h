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

#ifndef CORE_MEM_FIT_H
#define CORE_MEM_FIT_H

#include "config/config.h"
#include "libc/stdint.h"
#include "core/mem/address.h"

__BEGIN_DECLS

typedef enum {
	FIT_MODE_BEST  = 1,
	FIT_MODE_WORST = 2,
	FIT_MODE_FIRST = 3
} fit_mode_t;

int   fit_init(fit_mode_t mode,
	       addr_t     base,
	       size_t     size);
int   fit_initialized(void);
void* fit_alloc(size_t size);
void  fit_free(void *ptr);
void  fit_fini(void);

__END_DECLS

#endif /* CORE_MEM_FIT_H */
