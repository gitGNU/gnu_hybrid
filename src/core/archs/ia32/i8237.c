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
#include "libc/string.h"
#include "libc/stdint.h"

#include "core/arch/cpu.h"
#include "core/arch/port.h"
#include "core/archs/common/cpu.h"
#include "core/dbg/debug.h"

#if CONFIG_I8237_DEBUG
#define dprintf(F,A...) printf("i8237: " F,##A)
#else
#define dprintf(F,A...)
#endif

int i8237_init(void)
{
	return 1;
}

int i8237_fini(void)
{
	return 1;
}
