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
#include "archs/arch.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libs/debug.h"
#include "dbg/debugger.h"
#include "libc++/string"
#include "drivers/bus.h"

#define BANNER          "bus: "

#if CONFIG_BUS_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

bus::bus(const std::string & name) :
        device(name)
{
        dprintf("Bus 0x%x created (name = '%s')\n", this, name_.c_str());
}

bus::~bus()
{
        dprintf("Bus 0x%x (name = '%s') destroyed\n", this, name_.c_str());
}

#if CONFIG_DEBUGGER
static dbg_result_t command_bus_on_execute(FILE * stream,
                                           int    argc,
                                           char * argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return  DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Bus:\n");

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(device,
                    "Show busses",
                    NULL,
                    NULL,
                    command_devices_on_execute,
                    NULL);
#endif
