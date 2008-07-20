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
#include "libc/stdio.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "archs/ia32/tss.h"
#include "archs/ia32/gdt.h"
#include "archs/ia32/asm.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

#if CONFIG_TSS_DEBUG
#define dprintf(F,A...) printf("tsc: " F,##A)
#else
#define dprintf(F,A...)
#endif

int tsc_init(void)
{
#if 0
	uint64_t start;
	uint64_t end;
	ulong_t  count;
#endif

	dprintf("Calibrating ...\n");

#if 0
	rdtscll(start);
	for (count = 0; (inb(0x61) & 0x20) == 0; count++) {
		continue;
	}
	rdtscll(end);
#endif

	return 1;
}

void tsc_fini(void)
{
}
