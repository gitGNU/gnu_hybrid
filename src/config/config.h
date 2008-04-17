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

#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

/* Other configurations could depend on the version */
#include "config/version.h"

/* The auto-configuration first */
#include "config/autoconfig.h"

/* Then tweak the auto-configuration values */
#include "config/autoconfig-tweaks.h"

#if KERNEL
/* User configuration is available only for kernel-space builds */
#include "config/userconfig.h"
#endif

/* Include a useful set on headers last */
#ifndef ASM
/* We don't need to include files that could bother the assembler ... */
#include "libcompiler/macro.h"
#include "libcompiler/consistency.h"
#include "libcompiler/cdefs.h"
#include "libcompiler/compiler.h"
#include "libc/endian.h"

#endif /* ASM */

#endif /* CONFIG_CONFIG_H */
