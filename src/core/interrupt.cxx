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

static int        nr_locks;
arch_irqs_state_t state;
static int        initialized;

ktl::vector<ktl::list<ktl::pair<interrupt_handler_t,
				void *> > > handlers(MAX_IRQ_VECTORS);

__BEGIN_DECLS

int interrupts_init(void)
{
	nr_locks    = 0;
	initialized = 1;

	// handlers.resize(MAX_IRQ_VECTORS);

	handlers.clear();

	// Start in locked state
	interrupts_lock();

	return 1;
}

// Called from lower layer
void interrupts_handler(uint_t vector)
{
	assert(vector < MAX_IRQ_VECTORS);

	if (handlers[vector].empty()) {
		dprintf("No handler(s) for vector %d\n", vector);
		return;
	}

	ktl::list<ktl::pair<interrupt_handler_t, void *> >::iterator iter;

	dprintf("Executing %d interrupt handler(s) for vector %d\n",
		handlers[vector].size(), vector);

	for (iter  = handlers[vector].begin();
	     iter != handlers[vector].end();
	     iter++) {
		dprintf("Executing handler 0x%p with opaque 0x%p\n",
			(*iter).first, (*iter).second);

		assert((*iter).first);

		((*iter).first)((*iter).second);
	}
}

int interrupt_enabled(uint_t vector)
{
	assert(vector < MAX_IRQ_VECTORS);

	missing();

	return 0;
}

int interrupt_enable(uint_t vector)
{
	assert(vector < MAX_IRQ_VECTORS);
	return arch_irq_unmask(vector);
}

int interrupt_disable(uint_t vector)
{
	assert(vector < MAX_IRQ_VECTORS);
	return arch_irq_mask(vector);
}

int interrupts_unlocked(void)
{
	assert(initialized);

	return ((nr_locks == 0) ? 1 : 0);
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

int interrupt_attach(uint_t              vector,
		     interrupt_handler_t handler,
		     void *              opaque)
{
	assert(initialized);
	assert(vector < MAX_IRQ_VECTORS);

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
	if (handlers[vector].size() == 1) {
		arch_irq_handler_set(vector, interrupts_handler);
	}

	interrupts_unlock();

	dprintf("Handler 0x%p attached to irq %d\n", handler, vector);

	return true;
}

int interrupt_detach(uint_t              vector,
		     interrupt_handler_t handler)
{
	assert(initialized);
	assert(vector < MAX_IRQ_VECTORS);

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
			if (handlers[vector].empty()) {
				arch_irq_handler_set(vector, NULL);
			}
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

__END_DECLS
