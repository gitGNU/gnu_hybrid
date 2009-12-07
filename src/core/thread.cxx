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
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libs/debug.h"
#include "dbg/debugger.h"
#include "core/task.h"
#include "core/thread.h"

/* CONFIG_MULTITHREADING */
/* CONFIG_MAX_THREADS_PER_PROCESS */

#define BANNER          "thread: "

#if CONFIG_PROCESS_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

thread::thread(thread::id_t        id,
               const std::string & name) :
        task(id, name)
{
        dprintf("Thread %d initialized\n", id_);
}

thread::~thread()
{
        dprintf("Thread %d finalized\n", id_);
}
