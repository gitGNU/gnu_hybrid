//
// Copyright (C) 2008, 2009 Francesco Salvestrini
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

#include "elklib.h"
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
#include "archs/boot/option.h"
#include "libc++/cstdlib"

#if CONFIG_INIT_DEBUG
#define dprintf(F,A...) printf("main: " F,##A)
#else
#define dprintf(F,A...)
#endif

timer_t timer1;
timer_t timer2;
timer_t timer3;

void timer_cb(void *)
{
        putchar('T');
}

/* We reach this point from init() */
int main(int argc, char * argv[])
{
#if CONFIG_INIT_DEBUG
        dprintf("Entering main(argc = %d, argv = %p)\n", argc, argv);
#else
        unused_argument(argc);
        unused_argument(argv);
#endif
        printf("Welcome to Hybrid kernel %s\n", PACKAGE_VERSION);
        printf("Please report bugs to <%s>\n", PACKAGE_BUGREPORT);

        printf("Using elklib %s\n", ELKLIB_VERSION);

        if (!interrupts_init()) {
                panic("Cannot initialize interrupts");
        }

        if (!dma_init()) {
                panic("Cannot initialize interrupts");
        }

        if (!timers_init()) {
                panic("Cannot initialize timers");
        }

        TIMER_FILL(&timer1, timer_cb, 0, 05000, TIMER_ONE_SHOT);
        if (!timer_add(&timer1)) {
                panic("Cannot add timer");
        }
        TIMER_FILL(&timer2, timer_cb, 0, 10000, TIMER_ONE_SHOT);
        if (!timer_add(&timer2)) {
                panic("Cannot add timer");
        }
        TIMER_FILL(&timer3, timer_cb, 0, 15000, TIMER_REPETITIVE);
        if (!timer_add(&timer3)) {
                panic("Cannot add timer");
        }

        interrupts_unlock();

        scheduler * sched;

        sched = new scheduler();
        assert(sched);

#define TEST 1
#if TEST
#include "core/delay.h"
        static int i = 0;

        while (1 != 0) {
                i++;
                if (i >= 15000) {
                        putchar('A');

                        i = 0;
                }
        }
#endif

#if CONFIG_DEBUGGER
        dbg_enter();
#endif

        delete sched;

        panic("This is a panic test ...");

        return 0;
}
