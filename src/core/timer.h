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

#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libs/list.h"

__BEGIN_DECLS

struct timer {
	void         (* callback)(void * data);
	void *       data;
	int          expiration; /* Relative expiration time */

#if CONFIG_TIMERS_DEBUG
	int          absolute;   /* Absolute expiration time */
#endif

	list_entry_t list;
};
typedef struct timer timer_t;

#define TIMER_EXPIRED(TIMER) (((TIMER)->expiration <= 0) ? 1 : 0)
#define TIMER_INIT(TIMER,CALLBACK,DATA,DELAY)	\
	__BEGIN_MACRO				\
	assert(TIMER);				\
	assert((DELAY) >= 0);			\
	(TIMER)->callback   = CALLBACK;		\
	(TIMER)->data       = DATA;		\
	(TIMER)->expiration = DELAY;		\
	LIST_INIT(&(TIMER->list));              \
	__END_MACRO
#define TIMER_GOOD(TIMER)			\
	((TIMER) &&				\
	 ((TIMER)->callback != NULL) &&		\
	 ((TIMER)->expiration >= 0))

int timers_init(void);
int timers_fini(void);
int timer_add(timer_t * timer);
int timer_remove(timer_t * timer);

__END_DECLS

#endif /* CORE_TIMER_H */
