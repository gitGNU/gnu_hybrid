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

#ifndef CORE_MUTEX_H
#define CORE_MUTEX_H

#include "config/config.h"
#include "core/semaphore.h"

__BEGIN_DECLS

typedef struct {
	semaphore_t semaphore;
#if CONFIG_MUTEX_DEBUG
#endif
} mutex_t;


/* Statically allocated mutexes */
int      mutex_init(mutex_t* mutex);
int      mutex_fini(mutex_t* mutex);

/* Dinamically allocated mutexes */
mutex_t* mutex_new(void);
void     mutex_delete(mutex_t* mutex);

void     mutex_lock(mutex_t* mutex);
int      mutex_locked(mutex_t* mutex);
void     mutex_unlock(mutex_t* mutex);
int      mutex_trylock(mutex_t* mutex);

__END_DECLS

#endif /* CORE_MUTEX_H */
