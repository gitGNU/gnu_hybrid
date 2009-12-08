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
#include "dbg/debug.h"
#include "dbg/debugger.h"
#include "libc++/string"
#include "drivers/driver.h"

#define BANNER          "driver: "

#if CONFIG_DRIVER_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

driver::driver(const std::string & name) :
        name_(name)
{
        dprintf("Driver 0x%x created (name = '%s')\n", this, name_.c_str());
}

driver::~driver()
{
        dprintf("Driver 0x%x destroyed\n", this);
}

std::string driver::name()
{
        return name_;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_driver_on_execute(FILE * stream,
                                               int    argc,
                                               char * argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return  DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Driver:\n");

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(driver,
                    "Show drivers",
                    NULL,
                    NULL,
                    command_drivers_on_execute,
                    NULL);
#endif
