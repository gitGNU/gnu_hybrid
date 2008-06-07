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

#ifndef CORE_BFD_BFD_H
#define CORE_BFD_BFD_H

#include "config/config.h"
#include "libc/stdint.h"
#include "boot/bootinfo.h"

__BEGIN_DECLS

int  bfd_init(void);

int  bfd_config_kernel(bootinfo_t* bootinfo);
int  bfd_config_modules(bootinfo_t* bootinfo);

int  bfd_symbol_reverse_lookup(void*  address,
			       char*  buffer,
			       size_t length,
			       void** base);
int  bfd_symbols_foreach(int (* callback)(const char*   name,
					  unsigned long address));
int  bfd_images_foreach(int (* callback)(const char* name));

void bfd_fini(void);

__END_DECLS

#endif /* CORE_BFD_BFD_H */
