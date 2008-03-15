/*
 * Copyright (C) 2008 Francesco Salvestrini
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "config/config.h"
#include "core/archs/arch.h"
#include "core/spinlock.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"

void spinlock_init(spinlock_t* spinlock)
{
	assert(spinlock);

	*spinlock = 0;
}

void spinlock_acquire(spinlock_t* spinlock)
{
	int ret;

	assert(spinlock);
	assert(!arch_irqs_enabled());

	ret = arch_atomic_set((int *) spinlock, 1);
#if CONFIG_SPINLOCK_DEBUG
	if (ret == 1) {
		panic("Attempt to acquire and already acquired spinlock");
	}
#endif
}

void spinlock_release(spinlock_t* spinlock)
{
	int ret;

	assert(spinlock);
	assert(!arch_irqs_enabled());

	ret = arch_atomic_set((int *) spinlock, 0);
#if CONFIG_SPINLOCK_DEBUG
	if (ret == 0) {
		panic("Attempt to release and already released spinlock");
	}
#endif
}

void spinlock_fini(spinlock_t* spinlock)
{
	assert(spinlock);
	assert(*spinlock == 0);
}
