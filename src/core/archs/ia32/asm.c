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
#include "core/arch/asm.h"
#include "core/arch/gdt.h"

/* Function to see if a specific flag is changeable */
static int is_flag_changeable(uint32_t flag)
{
	uint32_t f1, f2;

	__asm__("pushfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"movl %0,%1\n\t"
		"xorl %2,%0\n\t"
		"pushl %0\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"popfl\n\t"
		: "=&r" (f1), "=&r" (f2)
		: "ir" (flag));

	return ((f1 ^ f2) & flag) != 0;
}

int have_cpuid(void)
{
	/* Simply use the CPUID detection flag */
	return (is_flag_changeable(EFLAGS_ID));
}

#if 0
boolean cpuHasMSR()
{
	dword a,d;
	cpuid(1,&a,&d);
	return d&CPUID_FLAG_MSR;
}

void cpuGetMSR(dword msr, dword *lo, dword *hi)
{
	__asm__ volatile ("rdsmr" : "=a"(*lo),"=d"(*hi),"c"(msr));
}

void cpuSetMSR(dword msr, dword lo, dword hi)
{
	__asm__ volatile ("wrsmr" : "=a"(lo),"=d"(hi),"c"(msr));
}
#endif

void cpuid(unsigned int   op,
	   unsigned int * eax,
	   unsigned int * ebx,
	   unsigned int * ecx,
	   unsigned int * edx)
{
	__asm__ volatile ("cpuid"
			  : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
			  : "0" (op), "c" (0));
}

unsigned int cpuid_eax(unsigned int op)
{
	unsigned int eax;

	__asm__ volatile ("cpuid"
			  : "=a" (eax)
			  : "0"  (op)
			  : "bx", "cx", "dx");

	return eax;
}

unsigned int cpuid_ebx(unsigned int op)
{
	unsigned int eax, ebx;

	__asm__ volatile ("cpuid"
			  : "=a" (eax), "=b" (ebx)
			  : "0"  (op)
			  : "cx", "dx" );

	return ebx;
}

unsigned int cpuid_ecx(unsigned int op)
{
	unsigned int eax, ecx;

	__asm__ volatile ("cpuid"
			  : "=a" (eax), "=c" (ecx)
			  : "0"  (op)
			  : "bx", "dx" );

	return ecx;
}

unsigned int cpuid_edx(unsigned int op)
{
	unsigned int eax, edx;

	__asm__ volatile ("cpuid"
			  : "=a" (eax), "=d" (edx)
			  : "0"  (op)
			  : "bx", "cx");

	return edx;
}

unsigned long eflags_get(void)
{
	unsigned long ef;

	__asm__ volatile ("pushf; popl %0" : "=r" (ef));

	return ef;
}

void eflags_set(unsigned long value)
{
	__asm__ volatile ("\tpushl %0\n"
			  "\tpopfl\n"
			  :
			  :"r" (value));
}

void lidt(void * idt_ptr)
{
	__asm__ volatile ("\tlidt (%%eax)\n"
			  :
			  :"a" (idt_ptr));
}

void lgdt(void * gdt_ptr)
{
	__asm__ volatile ("\tlgdt (%%eax)\n"
			  "\tjmp %1, $1f\n"
			  "\t1:\n"
			  "\tmovw %2,   %%ax\n"
			  "\tmovw %%ax, %%ss\n"
			  "\tmovw %%ax, %%ds\n"
			  "\tmovw %%ax, %%es\n"
			  "\tmovw %%ax, %%fs\n"
			  "\tmovw %%ax, %%gs\n"
			  :
			  :"a" (gdt_ptr),
			  "i" (SEGMENT_BUILDER(0, 0, SEGMENT_KERNEL_CODE)),
			  "i" (SEGMENT_BUILDER(0, 0, SEGMENT_KERNEL_DATA)));
}

unsigned long cr4_get(void)
{
	register unsigned long cr4;

	__asm__ volatile ("movl %%cr4, %0" : "=r" (cr4) : );

	return cr4;
}

void cr4_set(unsigned long cr4)
{
	__asm__ volatile ("movl %0, %%cr4" : : "r" (cr4));
}

unsigned long cr3_get(void)
{
	register unsigned long cr3;

	__asm__ volatile ("movl %%cr3, %0" : "=r" (cr3) : );

	return cr3;
}

void cr3_set(unsigned long cr3)
{
	__asm__ volatile ("movl %0, %%cr3" : : "r" (cr3));
}

unsigned long cr2_get(void)
{
	register unsigned long cr2;

	__asm__ volatile ("movl %%cr2, %0" : "=r" (cr2) : );

	return cr2;
}

void cr2_set(unsigned long cr2)
{
	__asm__ volatile ("movl %0, %%cr2" : : "r" (cr2));
}

unsigned long cr0_get(void)
{
	register unsigned long cr0;

	__asm__ volatile ("movl %%cr0, %0" : "=r" (cr0) : );

	return cr0;
}

void cr0_set(unsigned long cr0)
{
	__asm__ volatile ("movl %0, %%cr0" : : "r" (cr0));
}

void cli(void)
{
	__asm__ ("cli" : : );
}

void sti(void)
{
	__asm__ ("sti" : : );
}

void hlt(void)
{
	__asm__ ("hlt" : : );
}

#if 0 /* Reenable when needed ... */
void  cld(void)
{
	__asm__ ("cld" : : );
}

void std(void)
{
	__asm__ ("std" : : );
}

void clts(void)
{
	__asm__ ("clts" : : );
}
#endif

uint16_t ldt_get(void)
{
	uint16_t ldt;

	__asm__ ("sldt %0" : "=rm" (ldt));

	return ldt;
}

void ldt_set(uint16_t ldt)
{
	__asm__ ("lldt %0" : : "rm" (ldt));
}

uint16_t tr_get(void)
{
	uint16_t tr;

	__asm__ ("str %0" : "=rm" (tr));

	return tr;
}

void tr_set(uint16_t tr)
{
	__asm__ ("ltr %0" : : "rm" (tr));
}

void jmp_1f(void)
{
	__asm__ ("jmp 1f\n1:");
}
