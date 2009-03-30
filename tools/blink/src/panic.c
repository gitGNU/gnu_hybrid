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

#include "config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stdarg.h"
#include "libc/unistd.h"
#include "archs/arch.h"

#define MAX_STACK_LEVELS  32

static unsigned int backtrace[MAX_STACK_LEVELS];
extern void         _start;

void arch_panic(const char* message)
{
	static int   panic_in_progress = 0;
        unsigned int frames;

	panic_in_progress++;
	if (panic_in_progress > 1) {
                printf("Panic in progress ...\n");
		return;
	}

	/* Print the message (if any) */
	if (!message) {
		message = "EMPTY ???";
	}
	printf("Kernel panic: %s\n", message);

	frames = arch_backtrace_store(backtrace, MAX_STACK_LEVELS);
        if (frames == 0) {
                printf("No backtrace available ...\n");
        } else {
                int i;

                for (i = 0; i < frames; i++) {
                        unsigned int delta;
                        char *       symbol = "?";

                        /* _start is the base address */
                        delta = backtrace[i] - (unsigned int) &_start;
                        if (delta) {
                                printf("  %p <%s+0x%x>\n",
                                       backtrace[i], symbol, delta);
                        } else {
                                /* Huh ... hang in function call ? */
                                printf("  %p <%s>\n",
                                       backtrace[i], symbol);
                        }
                }
        }

	panic_in_progress--;

        arch_halt();
        arch_reset();

	printf("Cannot halt or reset the hardware ...\n");
        for (;;) { /* hmmmm .... */ }
}
