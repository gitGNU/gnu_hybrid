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
#include "core/scheduler.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"
#include "core/dbg/debugger/debugger.h"
#include "core/process.h"
#include "libc++/cstdio"
#include "libc++/list"

#define BANNER          "scheduler: "

#if CONFIG_SCHEDULER_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static ktl::list<process_t *> processes;

int scheduler_init(void)
{
	dprintf("Initializing scheduler\n");

	processes.clear();

	dprintf("Initialized\n");

	return 1;
}

void scheduler_run(void)
{
	process_t * & process = processes.front();
	assert(process);

	processes.pop_front();

	dprintf("Scheduling process $d\n", process->id);
}

int scheduler_fini(void)
{
	dprintf("Finalizing schedulers\n");

	if (!processes.empty()) {
		dprintf("Processess list not empty\n");
		return 0;
	}

	return 1;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_processes_on_execute(FILE* stream,
						 int   argc,
						 char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Processes:\n");

	ktl::list<process_t *>::iterator iter;
	for (iter = processes.begin(); iter != processes.end(); iter++) {
		fprintf(stream, "  0x%p %d\n",
			(*iter),
			(*iter)->id);
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(processes,
		    "Show processes",
		    NULL,
		    NULL,
		    command_processes_on_execute,
		    NULL);
#endif
