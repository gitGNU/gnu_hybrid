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
#include "core/arch/port.h"
#include "core/resource.h"

#if CONFIG_BOCHS_DEBUGGER

/*
 * NOTE:
 *     Port e9 output (put() method) is useful only when bochs is configured
 *     with --enable-port-e9-hack. The following lines have been extracted
 *     from bochs's config.h:
 *
 *     "... This enables writing to port 0xe9 and the output
 *      is sent to the console.  Reading from port 0xe9
 *      will return 0xe9 to let you know this is available ..."
 *
 * NOTE:
 *     See http://bochs.sourceforge.net/cgi-bin/lxr/source/iodev/iodebug.cc
 *     for more infos on bochs debugger ...
 *
 */

#define BOCHS_PORT 0xE9

RESOURCE_DECLARE(bochs,"Bochs port",RSRC_IO,BOCHS_PORT,BOCHS_PORT);

static int initialized = 0;

int bochs_init(void)
{
	if (port_in8(BOCHS_PORT) == BOCHS_PORT) {
		initialized = 1;
	} else {
		initialized = 0;
	}

	return initialized;
}

int bochs_putchar(int c)
{
	if (initialized) {
		port_out8(BOCHS_PORT, c);
		return (unsigned char) c;
	}

	return EOF;
}

void bochs_fini(void)
{
	if (initialized) {
		initialized = 0;
	}
}

#endif /* CONFIG_BOCHS_DEBUGGER */
