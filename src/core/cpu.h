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

#ifndef CORE_CPU_H
#define CORE_CPU_H

#include "config/config.h"
#include "libc++/vector"

class cpu {
public:
        cpu();
        ~cpu();

        void online();
        void offline();
        bool is_online();

private:
        bool online_;
};

// Singleton for all CPUs on the system
class cpus {
public:
        static cpus * instance();

protected:
        cpus();
        cpus(const cpus &);
        cpus & operator= (const cpus &);

private:
        static cpus *             instance_;
        static std::vector<cpu *> cpus_;
};

#endif // CORE_CPU_H
