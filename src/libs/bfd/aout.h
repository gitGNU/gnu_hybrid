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

#ifndef LIBS_BFD_AOUT_H
#define LIBS_BFD_AOUT_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libs/bfd/aout-format.h"

__BEGIN_DECLS

typedef struct {
	int num;
} aout_info_t;

int  aout_init(aout_info_t*  kernel_image,
	       unsigned long num,
	       unsigned long strsize,
	       unsigned long addr);
int  aout_symbol_reverse_lookup(aout_info_t* image,
				void*        address,
				char*        buffer,
				size_t       length,
				void**       base);
int  aout_symbols_foreach(aout_info_t* image,
			  int          (* callback)(const char*   name,
						    unsigned long address));
void aout_fini(void);

__END_DECLS

#endif /* LIBS_BFD_AOUT_H */
