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

#if CONFIG_REBOOT_ON_PANIC
static int panic_timeout = CONFIG_PANIC_TIMEOUT;
#endif /* REBOOT_ON_PANIC */

#define NO_PANIC_ON_PANIC 1

void arch_panic(const char* message)
{
#if NO_PANIC_ON_PANIC
	static int panic_in_progress = 0;
#endif

#if NO_PANIC_ON_PANIC
	/* A panic is in progress */
	panic_in_progress++;
	if (panic_in_progress > 1) {
		/* Don't panic too much, let the previous panic finish ;-) */
		return;
	}
#endif

	/* Print the message (if any) */
	if (!message) {
		message = "EMPTY ???";
	}
	printf("Kernel panic: %s\n", message);

#if 0
	backtrace_save();
	backtrace_show(stdout);
#endif

#if NO_PANIC_ON_PANIC
	/* We could panic again here ... is it correct ? */
	panic_in_progress--;
#endif

	//halt();
        do { } while (1);
}
