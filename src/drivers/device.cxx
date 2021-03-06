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
#include "drivers/device.h"

#define BANNER          "device: "

#if CONFIG_DRIVER_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

device::device(const std::string & name) :
        name_(name),
        driver_(0)
{
        dprintf("Device 0x%x created (name = '%s')\n", this, name_.c_str());
}

device::~device()
{
        dprintf("Device 0x%x (name = '%s') destroyed\n", this, name_.c_str());
}

std::string device::name()
{
        return name_;
}

void device::attach(driver * d)
{
        assert(d);

        driver_ = d;

        dprintf("Driver 0x%x (name = '%s') attached to device 0x%x\n",
                driver_, driver_->name().c_str(), this);
}

void device::detach()
{
        driver_ = 0;

        dprintf("Driver detached from device device 0x%x\n", this);
}

#if CONFIG_DEBUGGER
static dbg_result_t command_device_on_execute(FILE * stream,
                                              int    argc,
                                              char * argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return  DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Device:\n");

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(device,
                    "Show devices",
                    NULL,
                    NULL,
                    command_devices_on_execute,
                    NULL);
#endif
