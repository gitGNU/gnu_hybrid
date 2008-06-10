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
#include "libc/stddef.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

#if CONFIG_DEBUGGER

#if CONFIG_DEBUGGER_DEBUG
#define dprintf(F,A...)  printf(BANNER F,##A)
#define dputs(S)         puts(S)
#else
#define dprintf(F,A...)
#define dputs(S)
#endif

#define DBG_COLUMNS_MIN     10
#define DBG_COLUMNS_MAX     1000
#define DBG_COLUMNS_DEFAULT 80

#define DBG_LINES_MIN       1
#define DBG_LINES_MAX       1000
#define DBG_LINES_DEFAULT   25

DBG_VARIABLE_DECLARE(columns,
		     "Controls debugger console columns",
		     DBG_COLUMNS_MIN, DBG_COLUMNS_MAX, DBG_COLUMNS_DEFAULT,
		     DBG_VAR_RW,
		     NULL,
		     NULL,
		     NULL,
		     NULL);

DBG_VARIABLE_DECLARE(lines,
		     "Controls debugger console lines",
		     DBG_LINES_MIN, DBG_LINES_MAX, DBG_LINES_DEFAULT,
		     DBG_VAR_RW,
		     NULL,
		     NULL,
		     NULL,
		     NULL);

DBG_VARIABLE_DECLARE(case,
		     "Controls the debugger insensitiveness",
		     0, 1, 1 /* Case sensitiveness enabled by default */,
		     DBG_VAR_RW,
		     NULL,
		     NULL,
		     NULL,
		     NULL);

#endif /* CONFIG_DEBUGGER */
