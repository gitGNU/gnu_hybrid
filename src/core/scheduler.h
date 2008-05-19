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

#ifndef CORE_SCHEDULER_H
#define CORE_SCHEDULER_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

struct process {
	uint_t id;
};
typedef struct process process_t;

int  scheduler_init(void);
void scheduler_lock(void);
void scheduler_run(void);
int  scheduler_fini(void);

__END_DECLS

#endif /* CORE_SCHEDULER_H */
