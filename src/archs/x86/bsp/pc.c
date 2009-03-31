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

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "archs/arch.h"
#include "archs/x86/asm.h"
#include "archs/x86/port.h"
#include "archs/x86/idt.h"
#include "archs/x86/irq.h"
#include "archs/x86/i8237.h"
#include "archs/x86/i8253.h"
#include "libs/debug.h"

#if CONFIG_ARCH_PC

int arch_bootinfo_fix(bootinfo_t * bi)
{
#if 1
	unused_argument(bi);
#else
	int i;

	assert(bi);

	/* Remove lower memory */
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		if ((bi->mem[i].type == BOOTINFO_MEM_RAM) &&
		    (bi->mem[i].base < 1024 * 1024)) {
			/* XXX FIXME: Awful hack */
			bi->mem[i].type = BOOTINFO_MEM_ROM;
		}
	}
#endif

	return 1;
}

void arch_halt(void)
{
	irq_disable();

	hlt();
}

void arch_power_off(void)
{
	missing();

	arch_halt();
}

void arch_reset(void)
{
	/* Clean-up the IDT in order to cause a triple-fault */
	irq_disable();
	idt_clear();
	irq_enable();

	/* Are we ready for the triple-fault ? */
	__asm__ volatile ("int $0x03\n");

	/* We shouldn't reach this point ... */
	arch_halt();
}

void arch_irqs_enable(void)
{
	irq_enable();
}

void arch_irqs_disable(void)
{
	irq_disable();
}

int arch_irq_mask(uint_t irq)
{
	irq_mask(irq);
	return 1;
}

int arch_irq_unmask(uint_t irq)
{
	irq_unmask(irq);
	return 1;
}

arch_irqs_state_t arch_irqs_state_get(void)
{
	return irq_state_get();
}

void arch_irqs_state_set(arch_irqs_state_t * state)
{
	assert(state);

	irq_state_set(state);
}

void arch_delay_ns(uint32_t ns)
{
	unused_argument(ns);
}

void arch_delay_us(uint32_t us)
{
	unused_argument(us);

#if 0
	ulong_t ticks;
	ulong_t s, e;

	ticks = us * (__this_cpu->khz / CONFIG_HZ);
	rdtscl(s);
	do {
		rep_nop();
		rdtscl(e);
	} while ((e - s) < ticks);
#endif
}

void arch_delay_ms(uint32_t ms)
{
	arch_delay_us(ms * 1000);
}

int arch_vm_pagesize(void)
{
	return CONFIG_PAGE_SIZE;
}

size_t arch_dma_channels(void)
{
	return i8327_channels();
}

size_t arch_dma_channel_size(uint_t channel)
{
	return i8327_channel_size(channel);
}

int arch_dma_start_read(uint_t  channel,
			paddr_t address,
			size_t  count)
{
	return i8237_start(channel, address, count, 1);
}

int arch_dma_start_write(uint_t  channel,
			 paddr_t address,
			 size_t  count)
{
	return i8237_start(channel, address, count, 0);
}

int arch_dma_stop(uint_t channel)
{
	return i8237_stop(channel);
}

void arch_cpu_icache_flush(cpu_t* cpu)
{
	assert(cpu);
	assert(cpu->arch.ops);

	if (cpu->arch.ops->icache_flush) {
		cpu->arch.ops->icache_flush(&cpu->arch);
	}
}

void arch_cpu_icache_sync(cpu_t* cpu)
{
	assert(cpu);
	assert(cpu->arch.ops);

	if (cpu->arch.ops->icache_sync) {
		cpu->arch.ops->icache_sync(&cpu->arch);
	}
}

void arch_cpu_dcache_flush(cpu_t* cpu)
{
	assert(cpu);
	assert(cpu->arch.ops);

	if (cpu->arch.ops->dcache_flush) {
		cpu->arch.ops->dcache_flush(&cpu->arch);
	}
}

void arch_cpu_dcache_sync(cpu_t* cpu)
{
	assert(cpu);
	assert(cpu->arch.ops);

	if (cpu->arch.ops->dcache_sync) {
		cpu->arch.ops->dcache_sync(&cpu->arch);
	}
}

void arch_context_switch(vaddr_t old_stack,
			 vaddr_t new_stack,
			 vaddr_t new_mm)
{
	unused_argument(old_stack);
	unused_argument(new_stack);
	unused_argument(new_mm);
}

size_t arch_timer_granularity(void)
{
	return i8253_frequency();
}

#endif
