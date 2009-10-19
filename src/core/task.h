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

#ifndef CORE_TASK_H
#define CORE_TASK_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/limits.h"
#include "libc++/string"

#define DEFAULT_TASK_PRIORITY (UINT_MAX / 2)

class task {
public:
        typedef uint_t id_t;
        typedef uint_t priority_t;

        typedef enum {
                INITIALIZING,
                RUNNING,
                PENDING,
                READY
        } state_t;

        task(task::id_t          id,
             const std::string & name) :
                id_(id),
                state_(INITIALIZING),
                name_(name),
                priority_(DEFAULT_TASK_PRIORITY) { }

        virtual ~task() { }

        task::id_t id() const {
                return id_;
        };

        task::priority_t priority() const {
                return priority_;
        };

        void priority(task::priority_t p) {
                priority_ = p;
        };

        const std::string & name() const {
                return name_;
        }

        task::state_t state(void) const {
                return state_;
        }

        void suspend() { };
        void resume()  { };
        void restart() { };

protected:
        task();

        task::id_t       id_;
        state_t          state_;
        std::string      name_;
        task::priority_t priority_;

private:
};

#endif // CORE_TASK_H
