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

#ifndef BIOS_H
#define BIOS_H

#include "config/config.h"

__BEGIN_DECLS

int            bios_init(void);
void           bios_fini(void);

typedef enum {
	BIOS_MACHINE_UNKNOWN = 0,
	BIOS_MACHINE_PS386,
	BIOS_MACHINE_PCAT
} bios_machine_t;

bios_machine_t bios_machine(void);

__END_DECLS

#endif /* BIOS_H */
