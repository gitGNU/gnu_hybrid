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
#include "core/process.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"
#include "core/process.h"
#include "core/thread.h"

/* CONFIG_MAX_THREADS_PER_PROCESS */

#if CONFIG_DEBUGGER
static dbg_result_t command_threads_on_execute(FILE* stream,
					       int   argc,
					       char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Threads:\n");

	missing();

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(threads,
		    "Show threads",
		    NULL,
		    NULL,
		    command_threads_on_execute,
		    NULL);
#endif