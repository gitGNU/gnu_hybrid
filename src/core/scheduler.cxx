// -*- c++ -*-

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

#include "config/config.h"
#include "libc++/cstdio"
#include "libc++/list"
#include "libs/debug.h"
#include "dbg/debugger.h"
#include "core/scheduler.h"
#include "core/process.h"
#include "core/thread.h"

#define BANNER          "scheduler: "

#if CONFIG_SEMAPHORE_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

scheduler::scheduler()
{
        processes_.clear();

        dprintf("Scheduler initialized\n");
}

scheduler::~scheduler()
{
        std::list<process *>::iterator iter;
        for (iter = processes_.begin(); iter != processes_.end(); iter++) {
                delete *iter;
        }

        dprintf("Scheduler finalized\n");
}

void scheduler::run()
{
        process * & process = processes_.front();
        assert(process);

        processes_.pop_front();

        dprintf("Scheduling process %d\n", process->id());
}

#if CONFIG_DEBUGGER
#if 0
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

        std::list<process *>::iterator iter;
        for (iter = processes.begin(); iter != processes.end(); iter++) {
                fprintf(stream, "  0x%p %d\n",
                        (*iter),
                        (*iter)->id());
        }

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(processes,
                    "Show processes",
                    NULL,
                    NULL,
                    command_processes_on_execute,
                    NULL);
#endif // 0
#endif
