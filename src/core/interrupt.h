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
//

#ifndef CORE_INTERRUPT_H
#define CORE_INTERRUPT_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

void interrupts_disable(void);
void interrupts_enable(void);
void interrupts_lock(void);
void interrupts_unlock(void);

__END_DECLS

#ifdef __cplusplus

int  interrupts_init(void);
void interrupts_fini(void);

typedef void (* interrupt_handler_t)(void * opaque);

bool interrupts_attach(uint_t              vector,
		       interrupt_handler_t handler,
		       void *              opaque);
bool interrupts_detach(uint_t              vector,
		       interrupt_handler_t handler);
#endif

#endif // CORE_INTERRUPT_H
