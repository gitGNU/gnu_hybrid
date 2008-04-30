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
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "core/timer.h"
#include "core/mutex.h"
#include "libs/list.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"
#include "core/dbg/debugger/debugger.h"

#define BANNER          "timer: "

#if CONFIG_TIMERS_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif


static LIST_HEAD(timers);
static size_t granularity;

#if 0
static void timers_update(void)
{
	timer_t * timer;

	dprintf("Updating timers\n");

	if (LIST_ISEMPTY(&timers)) {
		dprintf("List is empty, quitting update\n");
		return;
	}

	timer = LIST_ENTRY(&timers, timer_t, list);
	assert(timer);

	dprintf("Updating timer %p\n", timer);

	timer->expiration -= granularity;
	if (TIMER_EXPIRED(timer)) {
		dprintf("Timer %p is expired\n", timer);

		LIST_REMOVE(&timer->list);
		if (!timer->callback) {
			dprintf("Timer %p callback is empty\n", timer);
			return;
		}

		timer->callback(timer->data);
	}

	dprintf("Updated completed\n");
}
#endif

int timers_init(void)
{
	dprintf("Initializing timers\n");

	LIST_INIT(&timers);

	assert(LIST_ISEMPTY(&timers));

	granularity = arch_timer_granularity();
	assert(granularity > 0);

	dprintf("Timers granularity %d\n", granularity);

	dprintf("Initialized\n");

	return 1;
}

int timers_fini(void)
{
	dprintf("Finalizing timers\n");

	return 1;
}

int timer_add(timer_t * timer)
{
	list_entry_t * curr1;
	timer_t *      curr2;

	assert(timer);

	if (!TIMER_GOOD(timer)) {
		dprintf("Cannot add timer, no useful infos\n");
		return 0;
	}

	dprintf("Adding timer %p (expiration %d)\n", timer, timer->expiration);

	if (LIST_ISEMPTY(&timers)) {
		dprintf("Timers list is empty, adding timer at the head\n");

		LIST_INSERT_AFTER(&timers, &timer->list);

		assert(!LIST_ISEMPTY(&timers));

		return 1;
	}

	dprintf("Timers list not empty, adding somewhere\n");

	curr2 = NULL;

	LIST_FOREACH_FORWARD(&timers, curr1) {
		curr2 = LIST_ENTRY(curr1, timer_t, list);

		dprintf("Cursor %p expiration time is %d\n",
			curr2, curr2->expiration);

		if (curr2->expiration > timer->expiration) {
			dprintf("Cursor expiration time is bigger\n");
			break;
		}

		timer->expiration -= curr2->expiration;

		dprintf("Decrementing timer expiration (now %d)\n",
			timer->expiration);

		if (timer->expiration < 0) {
			dprintf("Timer expiration underflow while walking\n");
			timer->expiration = 0;
			break;
		}
	}

	assert(curr2 != NULL);

	if (LIST_ISLAST(&curr2->list)) {
		dprintf("Inserting timer %p before %p\n", timer, curr2);
		LIST_INSERT_BEFORE(&curr2->list, &timer->list);
	} else {
		dprintf("Inserting timer %p after %p\n", timer, curr2);
		LIST_INSERT_AFTER(&curr2->list, &timer->list);
	}

	return 1;
}

int timer_remove(timer_t * timer)
{
	int err;

	dprintf("Removing timer %p\n", timer);

	err = 0;

	assert(timer);

	if (LIST_ISEMPTY(&timers)) {
		dprintf("Timers list is empty, cannot remove\n");
		return err;
	} else {
		list_entry_t * curr1;
		timer_t *      curr2;

		LIST_FOREACH_FORWARD(&timers, curr1) {
			curr2 = LIST_ENTRY(curr1, timer_t, list);
			if (curr2 == timer) {
				dprintf("Timer found\n");
				if (curr2->list.next != &timers) {
					if (curr2->expiration > 0) {
						LIST_ENTRY(curr2->list.prev,
							   timer_t,
							   list)->expiration +=
							curr2->expiration;
					}
				}

				LIST_REMOVE(&curr2->list);

				return 1;
			}
		}
	}

	return err;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_timers_on_execute(FILE * stream,
					      int    argc,
					      char * argv[])
{
	list_entry_t * temp;

	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Timers:\n");
	LIST_FOREACH_FORWARD(&timers, temp) {
		timer_t * curr;

		curr = LIST_ENTRY(temp, timer_t, list);
		fprintf(stream, "  0x%p %d\n",
			curr->callback, curr->expiration);
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(timers,
		    "Show timers",
		    NULL,
		    NULL,
		    command_timers_on_execute,
		    NULL);
#endif
