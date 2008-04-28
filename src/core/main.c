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
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "core/dbg/debugger/debugger.h"
#include "core/dbg/debug.h"
#include "core/timer.h"
#include "core/scheduler.h"
#include "core/boot/option.h"
#include "core/build.h"

OPTION_DECLARE(test1,"this is a test1");
OPTION_DECLARE(test2,"this is a test2");
OPTION_DECLARE(test3,"this is a test3");

timer_t t1;
timer_t t2;
timer_t t3;

/* We reach this point from init() */
int main(int argc, char* argv[])
{
	printf("Entering main(argc = %d, argv = %p)\n", argc, argv);

	printf("Welcome to Hybrid kernel "
	       "v%d.%d.%d "
	       "%s "
	       "(build #%d, build on %s)\n",
	       VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO,
	       strlen(VERSION_EXTRA) ? "(" VERSION_EXTRA ")" : "",
	       BUILD_NUMBER, BUILD_DATE);

	printf("Option '%s' has value '%s'\n",
	       "test1", OPTION_VAR(test1));
	printf("Option '%s' has value '%s'\n",
	       "test2", OPTION_VAR(test2));
	printf("Option '%s' has value '%s'\n",
	       "test3", OPTION_VAR(test3));

	if (!timers_init()) {
		panic("Cannot initialize timers");
	}

	t1.callback   = (void (*)(void *)) 1;
	t1.expiration = 1;
	LIST_INIT(&t1.list);

	t2.callback   = (void (*)(void *)) 2;
	t2.expiration = 110;
	LIST_INIT(&t2.list);

	t3.callback   = (void (*)(void *)) 3;
	t3.expiration = 30;
	LIST_INIT(&t3.list);

	if (!timer_add(&t1) || !timer_add(&t2) || !timer_add(&t3)) {
		panic("Cannot add timer");
	}

#if CONFIG_DEBUGGER
	dbg_enter();
#endif

	if (!scheduler_init()) {
		panic("Cannot initialize scheduler");
	}

	(void) scheduler_fini();
	(void) timers_fini();

	panic("This is a panic test ...");

	return 0;
}
