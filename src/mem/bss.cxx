//
// Copyright (C) 2008 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "config/config.h"
#include "archs/linker.h"
#include "libc/ctype.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "mem/address.h"
#include "libs/debug.h"
#include "dbg/debugger.h"
#include "libs/bfd/bfd.h"

#include "libc++/cstring"

#define BANNER          "bss: "

#if CONFIG_BSS_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

__BEGIN_DECLS

#define BSS_USE_MEMSET 1

int bss_init(void)
{
#if !BSS_USE_MEMSET
	char * bss;
	size_t count;
#endif

	dprintf("Initializing BSS 0x%p-0x%p (%d bytes)\n",
		&_bss, &_ebss, &_ebss - &_bss);

	if (&_bss > &_ebss) {
		// Wrong bss addresses, return before wrecking ...
		return 0;
	}

#if BSS_USE_MEMSET
	assert(valid_bss_address((unsigned int) &_bss));
	assert(valid_bss_address((unsigned int) &_ebss));

	dprintf("Clearing %d bytes\n", &_ebss - &_bss);
	memset(&_bss, 0, &_ebss - &_bss);

#else
	count = 0;
	for (bss = ((char *) &_bss); bss < ((char *) &_ebss); bss++) {
		assert(valid_bss_address((unsigned int) bss));

		// dprintf("  0x%x\n", bss);

		*bss = 0;
		count++;
	}

	dprintf("Zeroed count = %d\n", count);
#endif

	dprintf("BSS initialized\n");

	return 1;
}

void bss_fini(void)
{
	// Nothing to do here ...
}

__END_DECLS
