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
#include "core/interrupt.h"
#include "libs/debug.h"
#include "core/panic.h"
#include "dbg/debugger.h"

#if CONFIG_INTERRUPTS_DEBUG
#define dprintf(F,A...) printf("interrupts: " F,##A)
#else
#define dprintf(F,A...)
#endif

static int        nr_locks;
arch_irqs_state_t state;
static int        initialized;

int interrupts_init(void)
{
	nr_locks    = 0;
	initialized = 1;

	return 1;
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
	arch_irqs_state_t curr_state;

	assert(initialized);
	assert(nr_locks >= 0);

	dprintf("Locking interrupts\n");

	curr_state = arch_irqs_state_get();
	arch_irqs_disable();

	nr_locks++;
	if (nr_locks == 1) {
		state = curr_state;
		dprintf("Interrupts locked\n");
	}
}

void interrupts_unlock(void)
{
	assert(initialized);
	assert(nr_locks > 0);

	dprintf("Unocking interrupts\n");

	nr_locks--;
	if (nr_locks == 0) {
		arch_irqs_state_set(&state);
		dprintf("Interrupts unlocked\n");
	}
}

void interrupts_fini(void)
{
	assert(initialized);

	nr_locks = 0;
	arch_irqs_disable();

	initialized = 0;
}
