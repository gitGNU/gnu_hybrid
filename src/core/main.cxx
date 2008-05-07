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
#include "libc++/cstdlib"

OPTION_DECLARE(test1,"this is a test1");
OPTION_DECLARE(test2,"this is a test2");
OPTION_DECLARE(test3,"this is a test3");

#define TEST_TIMERS 1

#if TEST_TIMERS
timer_t t1;
timer_t t2;
timer_t t3;
timer_t t4;
timer_t t5;
timer_t t6;
#endif

/* We reach this point from init() */
int main(int argc, char* argv[])
{
	timer_t * p;

	p = (timer_t *) malloc(sizeof(timer_t));
	free(p);
	p = (timer_t *) malloc(sizeof(timer_t));
	free(p);
	p = (timer_t *) malloc(sizeof(timer_t));
	free(p);
	p = (timer_t *) malloc(sizeof(timer_t));
	free(p);
	p = (timer_t *) malloc(sizeof(timer_t));
	free(p);
	p = (timer_t *) malloc(sizeof(timer_t));
	free(p);
	p = (timer_t *) malloc(sizeof(timer_t));
	free(p);

	p = new timer_t;
	delete p;
	p = new timer_t;
	delete p;
	p = new timer_t;
	delete p;
	p = new timer_t;
	delete p;
	p = new timer_t;
	delete p;
	p = new timer_t;
	delete p;
	p = new timer_t;
	delete p;


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

#if TEST_TIMERS
	t1.callback   = (void (*)(void *)) 1;
	t1.expiration = 1;

	t2.callback   = (void (*)(void *)) 2;
	t2.expiration = 20;

	t3.callback   = (void (*)(void *)) 3;
	t3.expiration = 300;

	t4.callback   = (void (*)(void *)) 4;
	t4.expiration = 4000;

	t5.callback   = (void (*)(void *)) 5;
	t5.expiration = 50000;

	t6.callback   = (void (*)(void *)) 6;
	t6.expiration = 600000;

	if (!timer_add(&t6)) {
		panic("Cannot add a timer");
	}
	if (!timer_add(&t5)) {
		panic("Cannot add a timer");
	}
	if (!timer_add(&t1)) {
		panic("Cannot add a timer");
	}
	if (!timer_add(&t2)) {
		panic("Cannot add a timer");
	}
	if (!timer_add(&t3)) {
		panic("Cannot add a timer");
	}
	if (!timer_add(&t4)) {
		panic("Cannot add a timer");
	}

#if CONFIG_DEBUGGER
	dbg_enter();
#endif

	timer_remove(&t6);
	timer_remove(&t1);
	timer_remove(&t2);
	timer_remove(&t4);
	timer_remove(&t3);
	timer_remove(&t5);

#if CONFIG_DEBUGGER
	dbg_enter();
#endif

#endif /* TEST_TIMERS */

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
