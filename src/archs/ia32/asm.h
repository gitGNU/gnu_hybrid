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

#ifndef ARCH_ASM_H
#define ARCH_ASM_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/param.h"

__BEGIN_DECLS

/*
 * FEATURES bits
 */
/* Intel-defined CPU features, CPUID level 0x00000001 (edx), word 0 */
#define X86_FEATURE_FPU         (0*32+ 0) /* Onboard FPU */
#define X86_FEATURE_VME         (0*32+ 1) /* Virtual Mode Extensions */
#define X86_FEATURE_DE          (0*32+ 2) /* Debugging Extensions */
#define X86_FEATURE_PSE         (0*32+ 3) /* Page Size Extensions */
#define X86_FEATURE_TSC         (0*32+ 4) /* Time Stamp Counter */
#define X86_FEATURE_MSR         (0*32+ 5) /* Model-Specific Registers, RDMSR, WRMSR */
#define X86_FEATURE_PAE         (0*32+ 6) /* Physical Address Extensions */
#define X86_FEATURE_MCE         (0*32+ 7) /* Machine Check Architecture */
#define X86_FEATURE_CX8         (0*32+ 8) /* CMPXCHG8 instruction */
#define X86_FEATURE_APIC        (0*32+ 9) /* Onboard APIC */
#define X86_FEATURE_SEP         (0*32+11) /* SYSENTER/SYSEXIT */
#define X86_FEATURE_MTRR        (0*32+12) /* Memory Type Range Registers */
#define X86_FEATURE_PGE         (0*32+13) /* Page Global Enable */
#define X86_FEATURE_MCA         (0*32+14) /* Machine Check Architecture */
#define X86_FEATURE_CMOV        (0*32+15) /* CMOV instruction (FCMOVCC and FCOMI too if FPU present) */
#define X86_FEATURE_PAT         (0*32+16) /* Page Attribute Table */
#define X86_FEATURE_PSE36       (0*32+17) /* 36-bit PSEs */
#define X86_FEATURE_PN          (0*32+18) /* Processor serial number */
#define X86_FEATURE_CLFLSH      (0*32+19) /* Supports the CLFLUSH instruction */
#define X86_FEATURE_DTES        (0*32+21) /* Debug Trace Store */
#define X86_FEATURE_ACPI        (0*32+22) /* ACPI via MSR */
#define X86_FEATURE_MMX         (0*32+23) /* Multimedia Extensions */
#define X86_FEATURE_FXSR        (0*32+24) /* FXSAVE and FXRSTOR instructions (fast save and restore */
					  /* of FPU context), and CR4.OSFXSR available */
#define X86_FEATURE_XMM         (0*32+25) /* Streaming SIMD Extensions */
#define X86_FEATURE_XMM2        (0*32+26) /* Streaming SIMD Extensions-2 */
#define X86_FEATURE_SELFSNOOP   (0*32+27) /* CPU self snoop */
#define X86_FEATURE_HT          (0*32+28) /* Hyper-Threading */
#define X86_FEATURE_ACC         (0*32+29) /* Automatic clock control */
#define X86_FEATURE_IA64        (0*32+30) /* IA-64 processor */

/* AMD-defined CPU features, CPUID level 0x80000001, word 1 */
/* Don't duplicate feature flags which are redundant with Intel! */
#define X86_FEATURE_SYSCALL     (1*32+11) /* SYSCALL/SYSRET */
#define X86_FEATURE_MP          (1*32+19) /* MP Capable. */
#define X86_FEATURE_NX          (1*32+20) /* Execute Disable */
#define X86_FEATURE_MMXEXT      (1*32+22) /* AMD MMX extensions */
#define X86_FEATURE_LM          (1*32+29) /* Long Mode (x86-64) */
#define X86_FEATURE_3DNOWEXT    (1*32+30) /* AMD 3DNow! extensions */
#define X86_FEATURE_3DNOW       (1*32+31) /* 3DNow! */

/* Transmeta-defined CPU features, CPUID level 0x80860001, word 2 */
#define X86_FEATURE_RECOVERY    (2*32+ 0) /* CPU in recovery mode */
#define X86_FEATURE_LONGRUN     (2*32+ 1) /* Longrun power control */
#define X86_FEATURE_LRTI        (2*32+ 3) /* LongRun table interface */

/* Other features, Linux-defined mapping, word 3 */
/* This range is used for feature bits which conflict or are synthesized */
#define X86_FEATURE_CXMMX       (3*32+ 0) /* Cyrix MMX extensions */
#define X86_FEATURE_K6_MTRR     (3*32+ 1) /* AMD K6 nonstandard MTRRs */
#define X86_FEATURE_CYRIX_ARR   (3*32+ 2) /* Cyrix ARRs (= MTRRs) */
#define X86_FEATURE_CENTAUR_MCR (3*32+ 3) /* Centaur MCRs (= MTRRs) */
/* cpu types for specific tunings: */
#define X86_FEATURE_K8          (3*32+ 4) /* Opteron, Athlon64 */
#define X86_FEATURE_K7          (3*32+ 5) /* Athlon */
#define X86_FEATURE_P3          (3*32+ 6) /* P3 */
#define X86_FEATURE_P4          (3*32+ 7) /* P4 */

/* Intel-defined CPU features, CPUID level 0x00000001 (ecx), word 4 */
#define X86_FEATURE_XMM3        (4*32+ 0) /* Streaming SIMD Extensions-3 */
#define X86_FEATURE_MWAIT       (4*32+ 3) /* Monitor/Mwait support */
#define X86_FEATURE_DSCPL       (4*32+ 4) /* CPL Qualified Debug Store */
#define X86_FEATURE_EST         (4*32+ 7) /* Enhanced SpeedStep */
#define X86_FEATURE_TM2         (4*32+ 8) /* Thermal Monitor 2 */
#define X86_FEATURE_CID         (4*32+10) /* Context ID */
#define X86_FEATURE_CX16        (4*32+13) /* CMPXCHG16B */
#define X86_FEATURE_XTPR        (4*32+14) /* Send Task Priority Messages */

/* VIA/Cyrix/Centaur-defined CPU features, CPUID level 0xC0000001, word 5 */
#define X86_FEATURE_XSTORE      (5*32+ 2) /* on-CPU RNG present (xstore insn) */
#define X86_FEATURE_XSTORE_EN   (5*32+ 3) /* on-CPU RNG enabled */
#define X86_FEATURE_XCRYPT      (5*32+ 6) /* on-CPU crypto (xcrypt insn) */
#define X86_FEATURE_XCRYPT_EN   (5*32+ 7) /* on-CPU crypto enabled */

/* More extended AMD flags: CPUID level 0x80000001, ecx, word 6 */
#define X86_FEATURE_LAHF_LM     (5*32+ 0) /* LAHF/SAHF in long mode */
#define X86_FEATURE_CMP_LEGACY  (5*32+ 1) /* If yes HyperThreading not valid */

#define cpu_has(CPU,BIT)					\
	isset((unsigned char *) (CPU)->arch.infos.features, (BIT))

#define cpu_has_fpu(CPU)            cpu_has((CPU),X86_FEATURE_FPU)
#define cpu_has_vme(CPU)            cpu_has((CPU),X86_FEATURE_VME)
#define cpu_has_de(CPU)             cpu_has((CPU),X86_FEATURE_DE)
#define cpu_has_pse(CPU)            cpu_has((CPU),X86_FEATURE_PSE)
#define cpu_has_tsc(CPU)            cpu_has((CPU),X86_FEATURE_TSC)
#define cpu_has_msr(CPU)            cpu_has((CPU),X86_FEATURE_MSR)
#define cpu_has_pae(CPU)            cpu_has((CPU),X86_FEATURE_PAE)
#define cpu_has_pge(CPU)            cpu_has((CPU),X86_FEATURE_PGE)
#define cpu_has_apic(CPU)           cpu_has((CPU),X86_FEATURE_APIC)
#define cpu_has_sep(CPU)            cpu_has((CPU),X86_FEATURE_SEP)
#define cpu_has_mtrr(CPU)           cpu_has((CPU),X86_FEATURE_MTRR)
#define cpu_has_mmx(CPU)            cpu_has((CPU),X86_FEATURE_MMX)
#define cpu_has_fxsr(CPU)           cpu_has((CPU),X86_FEATURE_FXSR)
#define cpu_has_xmm(CPU)            cpu_has((CPU),X86_FEATURE_XMM)
#define cpu_has_xmm2(CPU)           cpu_has((CPU),X86_FEATURE_XMM2)
#define cpu_has_xmm3(CPU)           cpu_has((CPU),X86_FEATURE_XMM3)
#define cpu_has_ht(CPU)             cpu_has((CPU),X86_FEATURE_HT)
#define cpu_has_mp(CPU)             cpu_has((CPU),X86_FEATURE_MP)
#define cpu_has_nx(CPU)             cpu_has((CPU),X86_FEATURE_NX)
#define cpu_has_k6_mtrr(CPU)        cpu_has((CPU),X86_FEATURE_K6_MTRR)
#define cpu_has_cyrix_arr(CPU)      cpu_has((CPU),X86_FEATURE_CYRIX_ARR)
#define cpu_has_centaur_mcr(CPU)    cpu_has((CPU),X86_FEATURE_CENTAUR_MCR)
#define cpu_has_xstore(CPU)         cpu_has((CPU),X86_FEATURE_XSTORE)
#define cpu_has_xstore_enabled(CPU) cpu_has((CPU),X86_FEATURE_XSTORE_EN)
#define cpu_has_xcrypt(CPU)         cpu_has((CPU),X86_FEATURE_XCRYPT)
#define cpu_has_xcrypt_enabled(CPU) cpu_has((CPU),X86_FEATURE_XCRYPT_EN)

int           have_cpuid(void);
void          cpuid(unsigned int   op,
		    unsigned int * eax,
		    unsigned int * ebx,
		    unsigned int * ecx,
		    unsigned int * edx);
unsigned int  cpuid_eax(unsigned int op);
unsigned int  cpuid_ebx(unsigned int op);
unsigned int  cpuid_ecx(unsigned int op);
unsigned int  cpuid_edx(unsigned int op);

/*
 * EFLAGS bits
 */
#define EFLAGS_CF    0x00000001 /* Carry Flag */
#define EFLAGS_PF    0x00000004 /* Parity Flag */
#define EFLAGS_AF    0x00000010 /* Auxillary carry Flag */
#define EFLAGS_ZF    0x00000040 /* Zero Flag */
#define EFLAGS_SF    0x00000080 /* Sign Flag */
#define EFLAGS_TF    0x00000100 /* Trap Flag */
#define EFLAGS_IF    0x00000200 /* Interrupt Flag */
#define EFLAGS_DF    0x00000400 /* Direction Flag */
#define EFLAGS_OF    0x00000800 /* Overflow Flag */
#define EFLAGS_IOPL  0x00003000 /* IOPL mask */
#define EFLAGS_NT    0x00004000 /* Nested Task */
#define EFLAGS_RF    0x00010000 /* Resume Flag */
#define EFLAGS_VM    0x00020000 /* Virtual Mode */
#define EFLAGS_AC    0x00040000 /* Alignment Check */
#define EFLAGS_VIF   0x00080000 /* Virtual Interrupt Flag */
#define EFLAGS_VIP   0x00100000 /* Virtual Interrupt Pending */
#define EFLAGS_ID    0x00200000 /* CPU ID detection flag */

unsigned long eflags_get(void);
void          eflags_set(unsigned long value);

/*
 * CR0 feature flags
 */
#define CR0_PG  0x80000000 /* Enable Paging */
#define CR0_CD  0x40000000 /* Cache Disable */
#define CR0_NW  0x20000000 /* No Write-through */
#define CR0_AM  0x00040000 /* Alignment check Mask */
#define CR0_WP  0x00010000 /* Write-Protect kernel access */
#define CR0_NE  0x00000020 /* handle Numeric Exceptions */
#define CR0_ET  0x00000010 /* Extension Type is 80387 coprocessor */
#define CR0_TS  0x00000008 /* Task Switch */
#define CR0_EM  0x00000004 /* EMulate coprocessor */
#define CR0_MP  0x00000002 /* Monitor coProcessor */
#define CR0_PE  0x00000001 /* Protected mode Enable */

/*
 * CR4 feature flags
 */
#define CR4_VME         0x0001 /* Enable vm86 extensions */
#define CR4_PVI         0x0002 /* Virtual interrupts flag enable */
#define CR4_TSD         0x0004 /* Disable time stamp at ipl 3 */
#define CR4_DE          0x0008 /* Enable debugging extensions */
#define CR4_PSE         0x0010 /* Enable page size extensions */
#define CR4_PAE         0x0020 /* Enable physical address extensions */
#define CR4_MCE         0x0040 /* Machine check enable */
#define CR4_PGE         0x0080 /* Enable global pages */
#define CR4_PCE         0x0100 /* Enable performance counters at ipl 3 */
#define CR4_OSFXSR      0x0200 /* Enable fast FPU save and restore */
#define CR4_OSXMMEXCPT  0x0400 /* Enable unmasked SSE exceptions */

unsigned long cr4_get(void);
void          cr4_set(unsigned long cr4);
unsigned long cr3_get(void);
void          cr3_set(unsigned long cr3);
unsigned long cr2_get(void);
void          cr2_set(unsigned long cr2);
unsigned long cr0_get(void);
void          cr0_set(unsigned long cr0);

void          lidt(void * idt_ptr);

void          lgdt(void * gdt_ptr);
void          lgdt_commit(void);

void          ltr(uint16_t selector);

void          cli(void);
void          sti(void);
void          hlt(void);

uint64_t      rdtsc(void);

__END_DECLS

#endif /* ARCH_ASM_H */
