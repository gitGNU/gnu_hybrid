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

#ifndef CORE_DBG_DEBUG_H
#define CORE_DBG_DEBUG_H

#include "config/config.h"
#include "libc/stdio.h"
#include "core/dbg/assert.h"

__BEGIN_DECLS

#define missing()					\
	__BEGIN_MACRO					\
	printf("Missing code in '%s' line %d\n",	\
	       __PRETTY_FUNCTION__,__LINE__);		\
	__END_MACRO

/* The user has the option to cut-off expect() calls */
#if CONFIG_EXPECT
#define expect(EXPRESSION)   assert(EXPRESSION)
#else
#define expect(EXPRESSION)
#endif

__END_DECLS

#endif /* CORE_DBG_DEBUG_H */
