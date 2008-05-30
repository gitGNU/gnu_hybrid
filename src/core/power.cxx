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
#include "archs/arch.h"
#include "core/syscall.h"
#include "core/dbg/panic.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"

static void hang(void)
{
	printf("Hanging, see the flames ...\n");

	for(;;) {
		/* Please wait, melt in progress ;-) */
	}
}

__BEGIN_DECLS

SYSCALL(7,halt);

int halt(void)
{
	printf("Halt in progress ...\n");

	arch_halt();

	printf("... Cannot halt!\n");

	hang(); /* We shouldn't reach this point */

	return 0;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_halt_on_execute(FILE* stream,
					    int   argc,
					    char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_WRONG_PARAMETERS;
	}

	unused_argument(argv);

	if (!halt()) {
		return DBG_RESULT_ERROR;
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(halt,
		    "Halts the system",
		    NULL,
		    NULL,
		    command_halt_on_execute,
		    NULL);
#endif

SYSCALL(2,poweroff);

int poweroff(void)
{
	printf("Power-off in progress ...\n");

	arch_poweroff();

	printf("... Cannot power-off!\n");

	hang(); /* We shouldn't reach this point */

	return 0;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_poweroff_on_execute(FILE* stream,
					    int   argc,
					    char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_WRONG_PARAMETERS;
	}

	unused_argument(argv);

	if (!poweroff()) {
		return DBG_RESULT_ERROR;
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(poweroff,
		    "Power-offs the system",
		    NULL,
		    NULL,
		    command_poweroff_on_execute,
		    NULL);
#endif

SYSCALL(3,reboot);

int reset(void)
{
	printf("Reboot in progress ...\n");

	arch_reset();

	printf("... Cannot reboot!\n");

	hang(); /* We shouldn't reach this point */

	return 0;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_reset_on_execute(FILE* stream,
					     int   argc,
					     char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_WRONG_PARAMETERS;
	}

	unused_argument(argv);

	if (!reset()) {
		return DBG_RESULT_ERROR;
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(reset,
		    "Resets the system",
		    NULL,
		    NULL,
		    command_reset_on_execute,
		    NULL);
#endif

__END_DECLS
