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
#include "elklib.h"
#include "libc/stdio.h"
#include "libc/assert.h"
#include "libcompiler/macro.h"

#define missing()						\
	__BEGIN_MACRO						\
	printf("Missing functionality in function at %s:%d\n",	\
	       __FILE__,__PRETTY_FUNCTION__,__LINE__);		\
	__END_MACRO

/*
 * NOTE:
 *     Nobody should remove bug() or unpredictable results will happen ...
 */
#define bug()					\
	__BEGIN_MACRO				\
	panic("Bug found in '%s' line %d",	\
	       __PRETTY_FUNCTION__,__LINE__);	\
	__END_MACRO

#endif /* CORE_DBG_DEBUG_H */
