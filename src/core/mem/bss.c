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
#include "core/archs/linker.h"
#include "libc/ctype.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "core/mem/address.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"
#include "core/power.h"
#include "core/bfd/bfd.h"

#define BANNER          "bss: "

#if CONFIG_BSS_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

int bss_init(void)
{
	char * bss;

	dprintf("Initializing BSS 0x%x:0x%x (%d bytes)\n",
		&_bss, &_ebss, &_ebss - &_bss);

	if (&_bss > &_ebss) {
		/* Wrong bss addresses, let us return before wrecking
		 * the whole thing ...
		 */
		return 0;
	}

	for (bss = ((char *) &_bss); bss < ((char *) &_ebss); bss++) {
		assert(valid_bss_address((unsigned int) bss));

		// dprintf("  0x%x\n", bss);

		*bss = 0;
	}

	dprintf("BSS initialized\n");

	return 1;
}

void bss_fini(void)
{
	/* Nothing to do here ... */
}
