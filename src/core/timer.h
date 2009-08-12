//
// Copyright (C) 2008, 2009 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//

#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

typedef enum {
	TIMER_ONE_SHOT,
	TIMER_REPETITIVE
} timer_type_t;

struct timer {
	void         (* callback)(void * data);
	void *       data;
	int          expiration; /* Relative expiration time */
	timer_type_t type;
#if CONFIG_TIMERS_DEBUG
	int          absolute;   /* Absolute expiration time */
	int          removable;
#endif
};
typedef struct timer timer_t;

#define TIMER_EXPIRED(TIMER) (((TIMER)->expiration <= 0) ? 1 : 0)
#define TIMER_DUMP(TIMER) {						\
	dprintf("  Timer 0x%p:\n", (TIMER));				\
	dprintf("    callback   = 0x%p\n", (TIMER)->callback);		\
	dprintf("    data       = 0x%p\n", (TIMER)->data);		\
	dprintf("    expiration = %d\n",   (TIMER)->expiration);	\
	dprintf("    type       = %d\n",   (TIMER)->type);		\
}
#define TIMERS_DUMP(TIMERS) {						\
	dprintf("Timers:\n");						\
	ktl::list<timer_t *>::iterator iter;				\
	for (iter = (TIMERS).begin(); iter != (TIMERS).end(); iter++) {	\
		TIMER_DUMP(*iter);					\
	}								\
}
#define TIMER_FILL(TIMER,CALLBACK,DATA,DELAY,TYPE)	\
	__BEGIN_MACRO					\
	(TIMER)->callback   = CALLBACK;			\
	(TIMER)->data       = DATA;			\
	(TIMER)->expiration = DELAY;			\
	(TIMER)->type       = TYPE;			\
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
