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

#include "config/config.h"
#include "core/archs/arch.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libs/queue.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"

#if 0

static int queue_init(queue_t* queue)
{
	assert(queue);

        queue->head  = queue->tail;
	queue->tail  = NULL;
        queue->count = 0;

        return 1;
}

static void queue_fini(queue_t* queue)
{
	assert(queue);
}

queue_t* queue_new(void)
{
	queue_t* tmp;
	
	tmp = (queue_t *) malloc(sizeof(queue_t));
	if (!tmp) {
		return NULL;
	}

	if (!queue_init(tmp)) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

void queue_delete(queue_t* queue)
{
	assert(queue);
	
	queue_fini(queue);
	free(queue);
}

int queue_enqueue(queue_t* queue,
		  void*    item)
{
        if (queue->tail == NULL) {
                queue->tail       = item;
                queue->head       = item;
        } else {
                queue->tail->next = item;
                queue->tail       = item;
        }
        item->next = NULL;

        queue->count++;

        return 0;
}

void* queue_dequeue(queue_t* queue)
{
	void* item;

        item = queue->head;
        if (queue->head != NULL) {
                queue->head = ((queue_t *) queue->head)->next;
	}
        if (queue->tail == item) {
                queue->tail = NULL;
	}
        if (item != NULL) {
                queue->count--;
	}

        return item;
}

void* queue_peek(queue_t* queue)
{
	assert(queue);

        return queue->head;
}

#endif
