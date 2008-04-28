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
#include "core/dbg/debug.h"

__BEGIN_DECLS

#define LIST_DEBUG 1
#if LIST_DEBUG
#define LIST_ASSERT(X) assert(X)
#else
#define LIST_ASSERT(X)
#endif

struct list_entry {
	struct list_entry * next, * prev;
};
typedef struct list_entry list_entry_t;

#define LIST_INITIALIZER(NAME)     { &(NAME), &(NAME) }
#define LIST_HEAD(NAME)					\
	list_entry_t NAME = LIST_INITIALIZER(NAME)

#define LIST_ISEMPTY(HEAD) ((HEAD)->prev == (HEAD)->next)

#define LIST_OFFSETOF(TYPE,MEMBER) ((unsigned int) (&(((TYPE *) 0)->MEMBER)))
#define LIST_ENTRY(POINTER,TYPE,MEMBER)					\
	((TYPE *) (((char *) POINTER) - LIST_OFFSETOF(TYPE,MEMBER)))

static inline void __list_init(list_entry_t * entry)
{
	LIST_ASSERT(entry);
	entry->prev = entry->next = entry;
}

#define LIST_INIT(HEAD)	__list_init(HEAD)

static inline void __list_insert(list_entry_t * new,
				 list_entry_t * prev,
				 list_entry_t * next)
{
	LIST_ASSERT(new);
	LIST_ASSERT(prev);
	LIST_ASSERT(next);

	next->prev = new;
	new->next  = next;
	new->prev  = prev;
	prev->next = new;
}

static inline void __list_insert_before(list_entry_t * entry,
					list_entry_t * new)
{
	LIST_ASSERT(entry);
	LIST_ASSERT(new);

	__list_insert(new, entry, entry->prev);
}

static inline void __list_insert_after(list_entry_t * entry,
				       list_entry_t * new)
{
	LIST_ASSERT(entry);
	LIST_ASSERT(new);

	__list_insert(new, entry, entry->next);
}

#define LIST_INSERT_BEFORE(ENTRY, NEW) __list_insert_before(ENTRY,NEW)
#define LIST_INSERT_AFTER(ENTRY, NEW)  __list_insert_after(ENTRY,NEW)

static inline void __list_remove(list_entry_t * prev,
				 list_entry_t * next)
{
	LIST_ASSERT(prev);
	LIST_ASSERT(next);

	next->prev = prev;
	prev->next = next;
}

#define LIST_REMOVE(ENTRY) __list_remove((ENTRY)->prev, (ENTRY)->next)

#define LIST_FOREACH_FORWARD(HEAD,CURRENT)	\
	for (CURRENT =  (HEAD)->next;		\
	     CURRENT != (HEAD);			\
	     CURRENT =  (CURRENT)->next)

#define LIST_FOREACH_BACKWARD(ENTRY,CURRENT)	\
	for (CURRENT =  (HEAD)->prev;		\
	     CURRENT != (HEAD);			\
	     CURRENT =  (CURRENT)->prev)

__END_DECLS

#endif /* LIBS_LIST_H */
