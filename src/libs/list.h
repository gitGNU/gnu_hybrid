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

#if 0

typedef struct list_link {
	struct list_link* next;
	struct list_link* prev;
} list_link_t;

typedef struct list {
	list_link_t link;
	uint_t      offset;
} list_t;

void  list_init(list_t* list);
void  list_fini(list_t* list);

void  list_init_etc(list_t* list, int offset);

void  list_add_link_to_head(list_t* list, void* link);
void  list_add_link_to_tail(list_t* list, void* link);
void  list_remove_link(void* _link);
void* list_get_next_item(list_t* list, void* item);
void* list_get_prev_item(list_t* list, void* item);
void* list_get_last_item(list_t* list);
void  list_add_item(list_t* list, void* item);
void  list_remove_item(list_t* list, void* item);
void  list_insert_item_before(list_t* list, void* before, void* item);
void* list_remove_head_item(list_t* list);
void* list_remove_tail_item(list_t* list);
void  list_move_to_list(list_t* sourceList, list_t* targetList);
bool  list_is_empty(list_t* list);
void* list_get_first_item(list_t* list);

#endif

__END_DECLS

#endif /* LIST_H */
