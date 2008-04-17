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

typedef struct list_entry {
	struct list_entry *next, *prev;
} list_entry_t;

typedef list_entry_t list_head_t;

#define LIST_INITIALIZER(N) { &(N), &(N) }
#define LIST_HEAD(N)        list_head_t N = LIST_INITIALIZER(N)

void list_init(list_head_t *list);
int  list_empty(list_entry_t *entry);
void list_insert_before(list_entry_t *entry,
			list_entry_t *new);
void list_insert_after(list_entry_t *entry,
		       list_entry_t *new);
void list_insert_head(list_head_t *head,
		      list_head_t *new);
void list_insert_tail(list_head_t *head,
		      list_head_t *new);
void list_remove(list_entry_t *entry);

#define LIST_FOREACH(POS, HEAD) \
	for (POS = HEAD; POS != HEAD; POS = (POS)->next)

__END_DECLS

#endif /* LIBS_LIST_H */
