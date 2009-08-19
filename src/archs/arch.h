/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
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

#ifndef ARCH_H
#define ARCH_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/stdlib.h"
#if ARCH_X86
#include "archs/x86/arch.h"
#endif
#include "boot/bootinfo.h"
#include "archs/common/cpu.h"
#include "mem/address.h"

__BEGIN_DECLS

/*
 * NOTE:
 *     Each architecture must define some symbols in their linker script. See
 *     linker.h file for further infos.
 */

/*
 * NOTE:
 *     We must have __BYTE_ORDER defined correctly in order to avoid ordering
 *     problems ...
 */
#if !defined(__BYTE_ORDER)
#error Byte order must be defined !
#endif

#if ((__BYTE_ORDER != __LITTLE_ENDIAN) &&       \
     (__BYTE_ORDER != __BIG_ENDIAN)    &&       \
     (__BYTE_ORDER != __PDP_ENDIAN))
#error Unsupported byte ordering ...
#endif

/*
 * NOTE:
 *     Each architecture MUST export the following functions
 */

int               arch_bootinfo_fix(bootinfo_t * bi);

/*
 * XXX FIXME:
 *     Maybe we should add an address_t typedef and update them
 *     accordingly ...
 */

/*
 * NOTE:
 *     The following arch_dbg_*() functions are useful for debugging
 *     purposes. The should be available as soon as possible so they have
 *     their _ini() and _fini() "methods" ... the real architecture _init will
 *     be executed later ...
 *
 * NOTE:
 *     When an error occours the arch_dbg_putchar() and arch_dbg_getchar()
 *     must return EOF
 */
int               arch_dbg_init(void);
int               arch_dbg_putchar(int c);
int               arch_dbg_getchar(void);
void              arch_dbg_fini(void);

void              arch_dbg_beep_on(uint_t frequency);
void              arch_dbg_beep_off(void);

int               arch_init(void);

int               arch_vm_pagesize(void);

void              arch_cpu_icache_flush(cpu_t * cpu);
void              arch_cpu_icache_sync(cpu_t * cpu);
void              arch_cpu_dcache_flush(cpu_t * cpu);
void              arch_cpu_dcache_sync(cpu_t * cpu);

int               arch_atomic_set(int * pointer, int value);
int               arch_atomic_add(int * pointer, int delta);
int               arch_atomic_sub(int * pointer, int delta);
int               arch_atomic_exchange(int * pointer, int new_value);
int               arch_atomic_test_and_set(int * pointer);

void              arch_context_switch(vaddr_t old_stack,
                                      vaddr_t new_stack,
                                      vaddr_t new_mm);

void              arch_halt(void);
void              arch_power_off(void);
void              arch_reset(void);

uint_t            arch_backtrace_store(uint_t * backtrace,
                                       uint_t   max_len);

void              arch_irqs_enable(void);
void              arch_irqs_disable(void);
int               arch_irq_mask(uint_t irq);
int               arch_irq_unmask(uint_t irq);
arch_irqs_state_t arch_irqs_state_get(void);
void              arch_irqs_state_set(arch_irqs_state_t * state);

typedef           void (* arch_irq_handler_t)(uint_t irq);
int               arch_irq_handler_set(uint_t             irq,
                                       arch_irq_handler_t handler);

void              arch_delay_ms(uint32_t ms);
void              arch_delay_us(uint32_t us);
void              arch_delay_ns(uint32_t ns);

size_t            arch_dma_channels(void);
size_t            arch_dma_channel_size(uint_t channel);
int               arch_dma_start_read(uint_t  channel,
                                      paddr_t address,
                                      size_t  count);
int               arch_dma_start_write(uint_t  channel,
                                       paddr_t address,
                                       size_t  count);
int               arch_dma_stop(uint_t channel);

size_t            arch_timer_granularity(void);

void              arch_fini(void);

__END_DECLS

#endif /* ARCH_H */
