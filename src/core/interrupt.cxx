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
#include "archs/arch.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc++/cstdio"
#include "libc++/vector"
#include "libc++/list"
#include "libc++/utility"
#include "core/interrupt.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

#if CONFIG_INTERRUPTS_DEBUG
#define dprintf(F,A...) printf("interrupts: " F,##A)
#else
#define dprintf(F,A...)
#endif

static int                                           nr_locks;
arch_irqs_state_t                                    state;
static int                                           initialized;

// XXX FIXME: Remove the hardwired constant
ktl::vector<ktl::list<ktl::pair<interrupt_handler_t,
				void *> > >          handlers(100);

int interrupts_init(void)
{
	nr_locks    = 0;
	initialized = 1;
	handlers.clear();

	return 1;
}

__BEGIN_DECLS

void interrupts_handler(uint_t vector)
{
	dprintf("Executing interrupt handlers for vector %d\n", vector);

	if (handlers[vector].empty()) {
		dprintf("No handlers for vector %d\n", vector);
		return;
	}

	ktl::list<ktl::pair<interrupt_handler_t, void *> >::iterator iter;

	dprintf("Handlers chain size %d\n", handlers[vector].size());

	for (iter  = handlers[vector].begin();
	     iter != handlers[vector].end();
	     iter++) {
		dprintf("Executing handler 0x%p with opaque 0x%p\n",
			(*iter).first, (*iter).second);

		assert((*iter).first);

		((*iter).first)((*iter).second);
	}
}

void interrupts_disable(void)
{
	assert(initialized);

	arch_irqs_disable();
	dprintf("Interrupts disabled\n");
}

void interrupts_enable(void)
{
	assert(initialized);

	dprintf("Enabling interrupts\n");
	arch_irqs_enable();
}

void interrupts_lock(void)
{
	assert(initialized);
	assert(nr_locks >= 0);

	dprintf("Locking interrupts\n");

	nr_locks++;
	if (nr_locks == 1) {
		state = arch_irqs_state_get();
		arch_irqs_disable();

		dprintf("Interrupts locked\n");
	} else {
		dprintf("Interrupts already locked\n");
	}
}

void interrupts_unlock(void)
{
	assert(initialized);
	assert(nr_locks > 0);

	dprintf("Unlocking interrupts\n");

	nr_locks--;
	if (nr_locks == 0) {
		arch_irqs_state_set(&state);
		arch_irqs_enable();

		dprintf("Interrupts unlocked\n");
	} else {
		dprintf("Interrupts already unlocked\n");
	}
}

__END_DECLS

bool interrupts_attach(uint_t              vector,
		       interrupt_handler_t handler,
		       void *              opaque)
{
	assert(initialized);

	if (!handler) {
		return false;
	}

	dprintf("Attaching handler 0x%p with opaque 0x%p to vector %d\n",
		handler, opaque, vector);

	interrupts_lock();

	ktl::pair<interrupt_handler_t, void *> temp(handler, opaque);
	assert(temp.first == handler);
	assert(temp.second == opaque);

	handlers[vector].push_back(temp);
	arch_irq_handler_set(vector, interrupts_handler);

	interrupts_unlock();

	dprintf("Handler 0x%p attached to irq %d\n", handler, vector);

	return true;
}

bool interrupts_detach(uint_t              vector,
		       interrupt_handler_t handler)
{
	assert(initialized);

	dprintf("Detaching handler 0x%p from vector %d\n",
		handler, vector);

	interrupts_lock();

	bool retval;

	retval = false;

	ktl::list<ktl::pair<interrupt_handler_t, void *> >::iterator iter;
	for (iter  = handlers[vector].begin();
	     iter != handlers[vector].end();
	     iter++) {
		if ((*iter).first == handler) {
			dprintf("Got handler to remove\n");
			handlers[vector].erase(iter);
			arch_irq_handler_set(vector, NULL);
			retval = true;
			break;
		}
	}

	interrupts_unlock();

	dprintf("Handler 0x%p detached %s from irq %d\n",
		handler, retval ? "successfully" : "unsuccessfully", vector);

	return retval;
}

void interrupts_fini(void)
{
	assert(initialized);

	nr_locks = 0;
	arch_irqs_disable();

	initialized = 0;
}
