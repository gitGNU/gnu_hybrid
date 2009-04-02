/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
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

#ifndef CONFIG_AUTOCONFIG_TWEAKS_H
#define CONFIG_AUTOCONFIG_TWEAKS_H

/*
 * NOTE:
 *     This file stores tweaks to the autoconf gathered defines placed in
 *     autoconfig.h before they get included by other include files.
 */

#ifndef CONFIG_AUTOCONFIG_H
#error This file must be included after config/autoconfig.h
#endif

#ifdef USERCONFIG_H
#error This file must be included before userconfig.h
#endif

/*
 * NOTE:
 *     The tweaks are placed in this file in order to not propagate them to
 *     other files in the source tree ... they must be confined to the kernel
 *     source tree only ...
 */

#endif /* CONFIG_AUTOCONFIG_TWEAKS_H */
