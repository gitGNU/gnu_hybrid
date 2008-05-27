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
#include "core/dbg/debug.h"
#include "core/arch/i8237.h"
#include "core/arch/dma.h"

#if CONFIG_ARCH_DMA_DEBUG
#define dprintf(F,A...) printf("dma: " F,##A)
#else
#define dprintf(F,A...)
#endif

int dma_init(void)
{
	return i8237_init();
}

void dma_fini(void)
{
	i8237_fini();
}

size_t arch_dma_channels(void)
{
	return i8327_channels();
}

size_t arch_dma_channel_size(uint_t channel)
{
	return i8327_channel_size(channel);
}

int arch_dma_start_read(uint_t channel,
			addr_t address,
			size_t count)
{
	return i8237_start(channel, address, count, 1);
}

int arch_dma_start_write(uint_t channel,
			 addr_t address,
			 size_t count)
{
	return i8237_start(channel, address, count, 0);
}

int arch_dma_stop(uint_t channel)
{
	return i8237_stop(channel);
}
