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
#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/stdint.h"
#include "archs/ia32/cpu.h"
#include "archs/ia32/port.h"
#include "archs/common/cpu.h"
#include "core/dbg/debug.h"
#include "mem/address.h"

#if CONFIG_I8237_DEBUG
#define dprintf(F,A...) printf("i8237: " F,##A)
#else
#define dprintf(F,A...)
#endif

#define NR_DMAS         7
#define DMA_MAX         (1024 * 64)
#define DMA_MASK        (DMA_MAX - 1)
#define DMA_ALIGN(n)    ((((addr_t)(n)) + DMA_MASK) & ~DMA_MASK)

struct dma_port {
	int mask;
	int mode;
	int clear;
	int addr;
	int count;
	int page;
};

static const struct dma_port dma_regs[] = {
	{ 0x0a, 0x0b, 0x0c, 0x00, 0x01, 0x87 }, /* Channel 0 */
	{ 0x0a, 0x0b, 0x0c, 0x02, 0x03, 0x83 }, /* Channel 1 */
	{ 0x0a, 0x0b, 0x0c, 0x04, 0x05, 0x81 }, /* Channel 2 */
	{ 0x0a, 0x0b, 0x0c, 0x06, 0x07, 0x82 }, /* Channel 3 */
	{ 0xd4, 0xd6, 0xd8, 0xc0, 0xc2, 0x8f }, /* Channel 4 (n/a) */
	{ 0xd4, 0xd6, 0xd8, 0xc4, 0xc6, 0x8b }, /* Channel 5 */
	{ 0xd4, 0xd6, 0xd8, 0xc8, 0xca, 0x89 }, /* Channel 6 */
	{ 0xd4, 0xd6, 0xd8, 0xcc, 0xce, 0x8a }, /* Channel 7 */
};

#define CHANNEL2BITS(C) (((C) < 4) ? (C) : (C) >> 2)

int i8237_start(uint_t channel,
		addr_t addr,
		size_t count,
		int    read)
{
	const struct dma_port * regs;
	uint_t                  bits, mode;

	assert(channel < NR_DMAS);

	channel = ((channel >= 4) ? (channel + 1) : channel);

	assert(channel != 4);
	assert(addr < 0xffffff);

	regs = &dma_regs[channel];
	bits = CHANNEL2BITS(channel);
	mode = read ? 0x44 : 0x48;
	count--;

	port_out8(regs->mask,  bits | 0x04);
	port_out8(regs->clear, 0x00);
	port_out8(regs->mode,  bits | mode);
	port_out8(regs->addr,  addr >> 0);
	port_out8(regs->addr,  addr >> 8);
	port_out8(regs->page,  addr >> 16);
	port_out8(regs->clear, 0x00);
	port_out8(regs->count, count >> 0);
	port_out8(regs->count, count >> 8);
	port_out8(regs->mask,  bits);

	return 1;
}

int i8237_stop(uint_t channel)
{
	assert(channel < NR_DMAS);

	channel = ((channel >= 4) ? (channel + 1) : channel);

	assert(channel != 4);

	port_out8(dma_regs[channel].mask, CHANNEL2BITS(channel) | 0x04);

	return 1;
}

size_t i8327_channels(void)
{
	return NR_DMAS - 1;
}

size_t i8327_channel_size(uint_t channel)
{
	assert(channel < NR_DMAS);

	return 0xffffff;
}

int i8237_init(void)
{
	return 1;
}

int i8237_fini(void)
{
	return 1;
}
