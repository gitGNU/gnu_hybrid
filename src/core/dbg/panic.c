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
#include "debug.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stdarg.h"
#include "libc/unistd.h"
#include "core/archs/arch.h"
#include "core/dbg/debug.h"
#include "core/dbg/backtrace.h"
#include "core/dbg/debugger/debugger.h"
#include "core/interrupt.h"
#include "core/power.h"
#include "core/dbg/morse.h"

#if CONFIG_REBOOT_ON_PANIC
static int panic_timeout = PANIC_TIMEOUT;
#endif /* REBOOT_ON_PANIC */

void arch_panic(const char* message)
{
	static int panic_in_progress = 0;

	interrupts_disable();

	/* Remember that a panic is in progress */
	panic_in_progress++;

	/* Print the message (if any) */
	if (!message) {
		message = "EMPTY ???";
	}
	printf("Kernel panic: %s\n", message);

	backtrace_save();
	backtrace_show(stdout);

	if (panic_in_progress > 1) {
		/* Don't panic too much, let the previous panic finish ;-) */
		printf("Panic inside panic, bailing out ...\n");
		interrupts_enable();
		return;
	}

#if (!CONFIG_DEBUGGER_ON_PANIC && \
     !CONFIG_HALT_ON_PANIC     && \
     !CONFIG_REBOOT_ON_PANIC)

	/*
	 * NOTE:
	 *     From the menu configuration there should be no possibilities
	 *     to reach this point ... one of CONFIG_HALT_ONPANIC,
	 *     CONFIG_REBOOT_ON_PANIC or DEBUGGER_ON_PANIC *MUST* be set.
	 */
#error No action defined when a panic occours ... fix the rules file ...
#endif

#if CONFIG_DEBUGGER_ON_PANIC
	if (!dbg_enter()) {
		printf("Cannot enter the integrated debugger ... \n");
	}
#endif

	/* We could panic again here ... is it correct ? */
	panic_in_progress--;

	morse("SOS");

	/* Reenable the interrupts to let other entities complete their work */
	interrupts_enable();

#if CONFIG_REBOOT_ON_PANIC
	if (panic_timeout > 0) {
		printf("Rebooting in %d seconds ...\n", panic_timeout);
		delay_ms(panic_timeout * 1000);

		reboot();
	}
#endif

#if CONFIG_HALT_ON_PANIC
	halt();
#endif
}
