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

#include "config/config.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "dbg/debugger.h"
#include "libs/debug.h"
#include "core/timer.h"
#include "core/interrupt.h"
#include "core/dma.h"
#include "core/cpu.h"
#include "core/scheduler.h"
#include "boot/option.h"
#include "core/build.h"
#include "libc++/cstdlib"

/* We reach this point from init() */
int main(int argc, char * argv[])
{
	printf("Entering main(argc = %d, argv = %p)\n", argc, argv);

	printf("Welcome to Hybrid kernel "
	       "v%d.%d.%d "
	       "%s "
	       "(build #%d, build on %s)\n",
	       VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO,
	       strlen(VERSION_EXTRA) ? "(" VERSION_EXTRA ")" : "",
	       BUILD_NUMBER, BUILD_DATE);

	if (!timers_init()) {
		panic("Cannot initialize timers");
	}

	if (!interrupts_init()) {
		panic("Cannot initialize interrupts");
	}
	interrupts_enable();

	if (!dma_init()) {
		panic("Cannot initialize interrupts");
	}

	scheduler * sched;

	sched = new scheduler();
	assert(sched);

#define TEST 1
#if TEST
#include "core/delay.h"
	while (1 != 0) {
#if 1
		printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
		printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
		printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
		printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
		printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
		printf("\n");
#endif
	}
#endif

#if CONFIG_DEBUGGER
	dbg_enter();
#endif

	delete sched;

	panic("This is a panic test ...");

	return 0;
}
