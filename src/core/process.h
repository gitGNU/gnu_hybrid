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

#ifndef CORE_PROCESS_H
#define CORE_PROCESS_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libc++/string"
#include "libc++/list"
#include "core/task.h"
#include "core/thread.h"

class process :
        public task {
public:
        typedef task::id_t id_t;

        process(process::id_t       id,
                const std::string & name);
        ~process();

protected:

private:
        std::list<thread *> threads_;
};

#endif // CORE_PROCESS_H
