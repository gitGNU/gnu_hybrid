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
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "libc/string.h"
#include "libs/bfd/formats/aout.h"
#include "libs/bfd/formats/aout-format.h"
#include "libs/debug.h"

#if CONFIG_AOUT

#define BANNER          "aout: "
#if CONFIG_AOUT_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

#define DEBUG_AOUT_STATS_DUMP 0
#define DEBUG_AOUT_SECTS_DUMP 0

int aout_symbol_reverse_lookup(aout_info_t* image,
			       void*        address,
			       char*        buffer,
			       size_t       length,
			       void**       base)
{
	unused_argument(image);
	unused_argument(address);
	unused_argument(buffer);
	unused_argument(length);
	unused_argument(base);

	return 1;
}

#if CONFIG_DEBUGGER /* XXX FIXME: Nobody else needs these functions ? */
int aout_symbols_foreach(aout_info_t* image,
			 int          (* callback)(const char*   name,
						   unsigned long address))
{
	unused_argument(image);
	unused_argument(callback);

	return 1;
}
#endif

int aout_init(aout_info_t*  kernel_image,
	      unsigned long num,
	      unsigned long strsize,
	      unsigned long addr)
{
	unused_argument(kernel_image);
	unused_argument(num);
	unused_argument(strsize);
	unused_argument(addr);

	return 1;
}

void aout_fini(void)
{
}

#endif /* CONFIG_AOUT */
