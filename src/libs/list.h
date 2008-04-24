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

__BEGIN_DECLS

struct list_entry {
	struct list_entry * next, * prev;
};
typedef struct list_entry list_entry_t;

#define LIST_INITIALIZER(NAME)     { & NAME, & NAME }
#define LIST_HEAD(NAME)            list_entry_t NAME = LIST_INITIALIZER(NAME)

#define LIST_INIT(HEAD)				\
	__BEGIN_MACRO				\
	(HEAD).next = (HEAD).prev = &(HEAD);	\
	__END_MACRO

#define LIST_ISEMPTY(HEAD) ((HEAD).prev == (HEAD).next)

#define LIST_OFFSETOF(TYPE,MEMBER) ((unsigned int) (&(((TYPE *) 0)->MEMBER)))
#define LIST_ENTRY(POINTER,TYPE,MEMBER)					\
	((TYPE *) (((char *) POINTER) - LIST_OFFSETOF(TYPE,MEMBER)))

#define LIST_INSERT_BEFORE(ENTRY,NEW)		\
	__BEGIN_MACRO				\
	(NEW).prev         = (ENTRY).prev;	\
	(NEW).next         = (ENTRY);		\
	(ENTRY).prev->next = (NEW);		\
	(ENTRY).prev       = (NEW);		\
	__END_MACRO

#define LIST_INSERT_AFTER(ENTRY,NEW)		\
	__BEGIN_MACRO				\
	(NEW).next         = (ENTRY).next;	\
	(NEW).prev         = (ENTRY);		\
	(ENTRY).next->prev = (NEW);		\
	(ENTRY).next       = (NEW);		\
	__END_MACRO

#define LIST_INSERT(ENTRY,NEW) LIST_INSERT_AFTER(ENTRY,NEW)

#define LIST_INSERT_TAIL(HEAD,NEW) LIST_INSERT_BEFORE(HEAD,NEW)

#define LIST_REMOVE(ENTRY)				\
	__BEGIN_MACRO					\
	if (!LIST_ISEMPTY(ENTRY)) {			\
		(ENTRY).prev->next = (ENTRY).next;	\
		(ENTRY).next->prev = (ENTRY).prev;	\
	} else {					\
		LIST_INIT(ENTRY);			\
	}						\
	__END_MACRO

#define LIST_FOREACH_FORWARD(ENTRY,CURRENT)				\
	for (CURRENT = ENTRY; CURRENT != ENTRY; CURRENT = (CURRENT)->next)

#define LIST_FOREACH_BACKWARD(ENTRY,CURRENT)				\
	for (CURRENT = ENTRY; CURRENT != ENTRY; CURRENT = (CURRENT)->prev)

__END_DECLS

#endif /* LIBS_LIST_H */
