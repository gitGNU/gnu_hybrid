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

#ifndef LIBS_LIST_H
#define LIBS_LIST_H

#include "config/config.h"
#include "src/core/dbg/debug.h"

__BEGIN_DECLS

typedef struct list_entry {
	struct list_entry *next, *prev;
} list_entry_t;

typedef list_entry_t list_head_t;

#define LIST_INITIALIZER(N)        { &(N), &(N) }
#define LIST_HEAD(N)               list_head_t N = LIST_INITIALIZER(N)
#define LIST_OFFSETOF(TYPE,MEMBER) (unsigned int)(&(((TYPE *) 0)->(MEMBER)))

/* Pointers arithmetics */
#define LIST_ENTRY(PTR,TYPE,MEMEBER)					\
	((TYPE *)(((char *) POINTER) - LIST_OFFSETOF(TYPE,MEMBER)))

#define LIST_INIT(ENTRY)			\
	__BEGIN_MACRO				\
	(ENTRY).next = (ENTRY).prev = &(ENTRY);	\
	__END_MACRO

#define LIST_ISEMPTY(ENTRY) ((ENTRY).prev == (ENTRY).next)

/*
 * XXX FIXME: Really crappy, initial work to be fixed ASAP
 */

#define LIST_INSERT_BEFORE(ENTRY,NEW)		\
	__BEGIN_MACRO				\
	(NEW).prev         = (ENTRY).prev;	\
	(NEW).next         = (ENTRY);		\
	(ENTRY).prev.next  = (NEW);		\
	(ENTRY).prev       = (NEW);		\
	__END_MACRO

#define LIST_INSERT_AFTER(ENTRY,NEW)		\
	__BEGIN_MACRO				\
	(NEW).next        = (ENTRY).next;	\
	(NEW).prev        = (ENTRY);		\
	(ENTRY).next.prev = (NEW);		\
	(ENTRY).next      = (NEW);		\
	__END_MACRO

#define LIST_REMOVE(ENTRY)				\
	__BEGIN_MACRO					\
	if (!LIST_ISEMPTY(ENTRY)) {			\
		(ENTRY).prev->next = (ENTRY).next;	\
		(ENTRY).next->prev = (ENTRY).prev;	\
	} else {					\
		LIST_INIT(ENTRY);			\
	}						\
	__END_MACRO

#define LIST_FOREACH_FORWARD(POS,ENTRY)				\
	for (POS = ENTRY; POS != ENTRY; ENTRY = (ENTRY)->next)

#define LIST_FOREACH_BACKWARD(POS,ENTRY)			\
	for (POS = ENTRY; POS != ENTRY; ENTRY = (ENTRY)->prev)

#define LIST_FOREACH(POS,HEAD) LIST_FOREACH_FORWARD(POS,HEAD)

__END_DECLS

#endif /* LIBS_LIST_H */
