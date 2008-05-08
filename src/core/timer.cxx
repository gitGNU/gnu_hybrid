//
// Copyright (C) 2008 Francesco Salvestrini
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

#include "config/config.h"
#include "core/archs/arch.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "core/timer.h"
#include "core/mutex.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"
#include "core/dbg/debugger/debugger.h"
#include "libc++/list"

#define BANNER          "timer: "

#if CONFIG_TIMERS_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static ktl::list<timer_t *> timers;
static size_t               granularity;

#if 0
static void timers_update(void)
{
	dprintf("Updating timers\n");

	if (timers.empty()) {
		dprintf("List is empty, quitting update\n");
		return;
	}

	timer_t * & timer = timers.front();
	assert(timer);

	dprintf("Updating timer 0x%p\n", timer);

	timer->expiration -= granularity;
	if (TIMER_EXPIRED(timer)) {
		dprintf("Timer 0x%p is expired\n", timer);

		timers.pop_front();

		if (!timer->callback) {
			dprintf("Timer 0x%p callback is empty\n", timer);
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

	timers.clear();

	granularity = arch_timer_granularity();
	assert(granularity > 0);

	dprintf("Timers granularity %d\n", granularity);

	dprintf("Initialized\n");

	return 1;
}

int timers_fini(void)
{
	dprintf("Finalizing timers\n");

	if (!timers.empty()) {
		dprintf("Timers list not empty\n");
		return 0;
	}

	return 1;
}

int timer_add(timer_t * timer)
{
	assert(timer);

#if CONFIG_TIMERS_DEBUG
	timer->absolute = timer->expiration;
#endif

	if (!TIMER_GOOD(timer)) {
		dprintf("Cannot add timer, no useful infos\n");
		return 0;
	}

	dprintf("Adding timer 0x%p (expiration %d)\n", timer, timer->expiration);

	if (timers.empty()) {
		dprintf("Timers list is empty, adding timer at the head\n");

		timers.push_front(timer);
		assert(!timers.empty());

		return 1;
	}

	dprintf("Timers list not empty, adding somewhere\n");

	ktl::list<timer_t *>::iterator iter;
	for (iter = timers.begin(); iter != timers.end(); iter++) {
		if ((*iter)->expiration > timer->expiration) {
			dprintf("Cursor expiration time is bigger\n");
			break;
		}

		timer->expiration -= (*iter)->expiration;

		dprintf("Decrementing timer expiration (now %d)\n",
			timer->expiration);

		if (timer->expiration < 0) {
			dprintf("Timer expiration underflow while walking\n");
			timer->expiration = 0;
			break;
		}
	}

	timers.insert(iter, timer);

	return 1;
}

int timer_remove(timer_t * timer)
{
	dprintf("Removing timer 0x%p\n", timer);

	assert(timer);

	if (timers.empty()) {
		dprintf("Timers list is empty, cannot remove\n");
		return 0;
	}

	ktl::list<timer_t *>::iterator iter1;

	for (iter1 = timers.begin(); iter1 != timers.end(); iter1++) {
		dprintf("Walking timer 0x%p\n", (*iter1));
		if ((*iter1) == timer) {
			dprintf("Got timer!\n");

			ktl::list<timer_t *>::iterator iter2;

			iter2 = iter1;
			iter2++;

			if (iter2 != timers.end()) {
				dprintf("Next timer is 0x%p\n", (*iter2));

				if ((*iter1)->expiration > 0) {
					dprintf("Timer 0x%p is not expired, "
						"fixing\n", (*iter1));
					(*iter2)->expiration +=
						(*iter1)->expiration;
				}
			}

			dprintf("Removing timer 0x%p from list\n", (*iter1));
			timers.erase(iter1);

			dprintf("Timer 0x%p removed successfully\n", timer);
			return 1;
		}
	}

	dprintf("Timer 0x%p not found\n", timer);

	return 0;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_timers_on_execute(FILE * stream,
					      int    argc,
					      char * argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Timers:\n");

	ktl::list<timer_t *>::iterator iter;
	for (iter = timers.begin(); iter != timers.end(); iter++) {
#if CONFIG_TIMERS_DEBUG
		fprintf(stream, "  0x%p 0x%p %d (%d)\n",
			(*iter),
			(*iter)->callback,
			(*iter)->expiration,
			(*iter)->absolute);
#else
		fprintf(stream, "  0x%p 0x%p %d\n",
			(*iter),
			(*iter)->callback,
			(*iter)->expiration);
#endif
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
