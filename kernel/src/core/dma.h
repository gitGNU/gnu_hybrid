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

#ifndef CORE_DMA_H
#define CORE_DMA_H

#include "config/config.h"
#include "libc/stdint.h"
#include "mem/address.h"

int    dma_init(void);
void   dma_fini(void);
size_t dma_channels(void);

typedef uint_t dma_channel_t;

bool  dma_start_read(dma_channel_t channel,
		     paddr_t       address,
		     size_t        count);
bool  dma_start_write(dma_channel_t channel,
		      paddr_t       address,
		      size_t        count);
bool  dma_stop(dma_channel_t channel);

__BEGIN_DECLS

__END_DECLS

#endif /* CORE_DMA_H */
