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
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "archs/arch.h"
#include "arch/asm.h"
#include "arch/port.h"
#include "arch/idt.h"
#include "arch/i8259.h"
#include "arch/i8237.h"
#include "arch/i8253.h"
#include "core/dbg/panic.h"
#include "core/dbg/debug.h"

#if CONFIG_ARCH_PC

void arch_halt(void)
{
	cli();
	hlt();
}

void arch_poweroff(void)
{
	missing();

	arch_halt();
}

void arch_reset(void)
{
	cli();
	idt_clear();
	sti();

	__asm__ volatile ("int $0x03\n");

	arch_halt();
}

void arch_irqs_enable(void)
{
	i8259_enable(0);
}

int arch_irqs_enabled(void)
{
	return (i8259_mask_get() ? 1 : 0);
}

void arch_irqs_disable(void)
{
	i8259_disable(0);
}

arch_irqs_state_t arch_irqs_state_get(void)
{
	return i8259_mask_get();
}

void arch_irqs_state_set(arch_irqs_state_t * state)
{
	assert(state);

	i8259_mask_set(*state);
}

/* Delay loop  */
void delay_loops(uint32_t loops)
{
	__asm__ volatile ("   movl %0, %%ecx\n\t"
			  "0: lahf\n\t"
			  "   dec  %%ecx\n\t"
			  "   jnz  0b\n\t"
			  :
			  : "r" (loops)
			  : "%ecx"
			  );
}

void arch_delay_ms(uint32_t ms)
{
	delay_loops(ms * __this_cpu->arch.loops_ms);
}

void arch_delay_us(uint32_t us)
{
	delay_loops((us * __this_cpu->arch.loops_ms) / 1024);
}

void arch_delay_ns(uint32_t ns)
{
	delay_loops((ns * __this_cpu->arch.loops_ms) / (1024 * 1024));
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

void arch_context_switch(uint_t* old_stack,
			 uint_t* new_stack,
			 uint_t* new_mm)
{
	unused_argument(old_stack);
	unused_argument(new_stack);
	unused_argument(new_mm);

#if 0
	__asm__ volatile (
			  pushal

			  movl    36(%esp), %eax
			  movl    %esp, (%eax)

			  movl    44(%esp), %eax
			  cmpl    $0x00, %eax
			  je      _no_mmu_switch

			  /*
			   * switching cr3 really *is* time-messing
			   */
			  movl    44(%esp), %eax
			  movl    %eax, %cr3

			  _no_mmu_switch:

			  movl    40(%esp), %eax
			  lss             (%eax), %esp

			  popal
			  ret
			  );
#endif
}

size_t arch_timer_granularity(void)
{
	return i8253_frequency();
}

#endif
