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

#ifndef LIST_H
#define LIST_H

#include "config/config.h"

__BEGIN_DECLS

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(N) { &(N), &(N)}
#define LIST_HEAD(N) \
	struct list_head N = LIST_HEAD_INIT(N)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

void list_add(struct list_head *head, struct list_head *new);
void list_add_tail(struct list_head *head, struct list_head *new);

__END_DECLS

#endif /* LIST_H */
