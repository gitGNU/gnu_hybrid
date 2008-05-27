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
#include "core/archs/arch.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc++/cstdio"
#include "libc++/vector"
#include "core/interrupt.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"
#include "core/dbg/debugger/debugger.h"
#include "core/dma.h"
#include "core/mem/address.h"

#if CONFIG_INTERRUPTS_DEBUG
#define dprintf(F,A...) printf("dma: " F,##A)
#else
#define dprintf(F,A...)
#endif

struct dma {
	uint_t index;
	bool   in_use;
	size_t size;
};
typedef struct dma dma_t;

ktl::vector<struct dma> channels;

int dma_init(void)
{
	channels.resize(arch_dma_channels(), dma_t());

	ktl::vector<struct dma>::iterator iter;
	uint_t                            index;
	uint_t                            count;

	for (iter  = channels.begin(), index = 0, count = 0;
	     iter != channels.end();
	     iter++) {
		size_t size;

		size = arch_dma_channel_size(index);
		if (size) {
			printf("Hardware failed to setup "
			       "channel %d correctly\n", index);
			index++;
		}
		(*iter).index  = index;
		(*iter).in_use = false;
		(*iter).size   = size;
		index++;
		count++;
	}
	dprintf("%d usable channels found\n", count);

	channels.resize(count, dma_t());

	return 1;
}

void dma_fini(void)
{
}

size_t dma_channels(void)
{
	return channels.size();
}

static bool attach(dma_channel_t channel)
{
	if (channel >= channels.size()) {
		return false;
	}
	if (channels[channel].in_use) {
		return false;
	}
	channels[channel].in_use = true;

	return true;
}

static bool detach(dma_channel_t channel)
{
	if (channel >= channels.size()) {
		return false;
	}
	if (!channels[channel].in_use) {
		return false;
	}
	channels[channel].in_use = false;

	return true;
}

bool dma_start_read(dma_channel_t channel,
		    addr_t        address,
		    size_t        count)
{
	if (!attach(channel)) {
		return false;
	}
	assert(channel < channels.size());

	struct dma & c = channels[channel];

	int retval;

	interrupts_lock();
	retval = arch_dma_start_read(c.index, virt_to_phys(address), count);
	interrupts_unlock();

	return retval ? true : false;
}

bool dma_start_write(dma_channel_t channel,
		     addr_t        address,
		     size_t        count)
{
	if (!attach(channel)) {
		return false;
	}
	assert(channel < channels.size());

	struct dma & c = channels[channel];

	int retval;

	interrupts_lock();
	retval = arch_dma_start_write(c.index, virt_to_phys(address), count);
	interrupts_unlock();

	return retval ? true : false;
}

bool dma_stop(dma_channel_t channel)
{
	if (!detach(channel)) {
		return false;
	}
	assert(channel < channels.size());

	struct dma & c = channels[channel];

	interrupts_lock();
	arch_dma_stop(c.index);
	interrupts_unlock();

	return true;
}
