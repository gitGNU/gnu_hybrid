/* -*- c -*- */

/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
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
#include "archs/common/cpu.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

cpu_t cpus[CONFIG_MAX_CPU_COUNT]; /* Placeholder of all CPUs data */

/*
 * XXX FIXME: We should update the cpu count on every on/off operations
 *            instead of computing them every time ...
 */
int arch_cpu_count(void)
{
        int count;
        int i;

        count = 0;
        for (i = 0; i < CONFIG_MAX_CPU_COUNT; i++) {
                if (cpus[i].online) {
                        count++;
                }
        }

        return count;
}

int arch_cpu_current(void)
{
        missing();

        return 0;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_cpus_on_execute(FILE* stream,
                                            int   argc,
                                            char* argv[])
{
        int i;

        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return  DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        for (i = 0; i < CONFIG_MAX_CPU_COUNT; i++) {
                fprintf(stream, "CPU%d:\n", i);
                fprintf(stream, "  Id    %d\n",
                        cpus[i].index);
                fprintf(stream, "  State %s\n",
                        (cpus[i].online ? "online" : "offline"));
        }

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(cpus,
                    "Show cpus",
                    NULL,
                    NULL,
                    command_cpus_on_execute,
                    NULL);

static dbg_result_t command_registers_on_execute(FILE* stream,
                                                 int   argc,
                                                 char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Registers:\n");

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(registers,
                    "Show registers values",
                    NULL,
                    NULL,
                    command_registers_on_execute,
                    NULL);
#endif
