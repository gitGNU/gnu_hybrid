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
#include "core/arch/asm.h"
#include "core/arch/cpu.h"
#include "core/archs/common/cpu.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "core/dbg/debug.h"

#if CONFIG_ARCH_CPU_DEBUG
#define dprintf(F,A...)   printf("CPU: " F,##A)
#else
#define dprintf(F,A...)
#endif

#if CONFIG_ARCH_CPU_VERBOSE
#define cprintf(C,F,A...) printf("CPU%d: " F,(C)->index,##A)
#else
#define cprintf(C,F,A...)
#endif

#if CONFIG_ARCH_CPU_INTEL
extern int  intel_infos(arch_cpu_t* cpu);
extern int  intel_cpu_init(arch_cpu_t* cpu);
extern void intel_cpu_fini(arch_cpu_t* cpu);
extern int  intel_cache_init(arch_cpu_t* cpu);
extern void intel_cache_fini(arch_cpu_t* cpu);
#endif /* CONFIG_ARCH_CPU_INTEL */

#if CONFIG_ARCH_CPU_AMD
extern int  amd_infos(arch_cpu_t* cpu);
extern int  amd_cpu_init(arch_cpu_t* cpu);
extern void amd_cpu_fini(arch_cpu_t* cpu);
extern int  amd_cache_init(arch_cpu_t* cpu);
extern void amd_cache_fini(arch_cpu_t* cpu);
#endif /* CONFIG_ARCH_CPU_AMD */

#if CONFIG_ARCH_CPU_GENERIC
extern int  generic_infos(arch_cpu_t* cpu);
extern int  generic_cpu_init(arch_cpu_t* cpu);
extern void generic_cpu_fini(arch_cpu_t* cpu);
extern int  generic_cache_init(arch_cpu_t* cpu);
extern void generic_cache_fini(arch_cpu_t* cpu);
#endif /* CONFIG_ARCH_CPU_GENERIC */

typedef struct {
	char*     ident;
	char*     vendor;
	cpu_ops_t ops;
} cpu_descriptor_t;

cpu_descriptor_t cpu_descriptors[] = {
#if CONFIG_ARCH_CPU_INTEL
	{
		"GenuineIntel",
		"Intel",
		{
			intel_infos,
			intel_cpu_init,
			intel_cpu_fini,
			intel_cache_init,
			intel_cache_fini,
			NULL,
			NULL,
			NULL,
			NULL,
		}
	},
#endif /* CONFIG_ARCH_CPU_INTEL */
#if CONFIG_ARCH_CPU_AMD
	{
		"AuthenticAMD",
		"AMD",
		{
			amd_infos,
			amd_cpu_init,
			amd_cpu_fini,
			amd_cache_init,
			amd_cache_fini,
			NULL,
			NULL,
			NULL,
			NULL,
		}
	},
#endif /* CONFIG_ARCH_CPU_AMD */
#if CONFIG_ARCH_CPU_GENERIC
	{
		NULL /* This is the fall-back case, it must be the last one */,
		"Generic",
		{
			generic_infos,
			generic_cpu_init,
			generic_cpu_fini,
			generic_cache_init,
			generic_cache_fini,
			NULL,
			NULL,
			NULL,
			NULL,
		}
	}
#endif /* CONFIG_ARCH_CPU_GENERIC */
};

/* Per CPU initializer */
int cpu_init(int indx)
{
	char              ident[13];
	unsigned int      eax, ebx, ecx, edx;
	cpu_t*            cpu;
	cpu_descriptor_t* descriptor;

	assert(indx >= 0 && indx < CONFIG_MAX_CPU_COUNT);

	/* Get the CPU descriptor and fill its index */
	cpu        = &cpus[indx];
	cpu->index = indx;

	/* Get level and manufacturer identifier string */
	cpuid(0, &eax, &ebx, &ecx, &edx);

	/* Fill CPU level */
	cpu->arch.level = eax;
	if (cpu->arch.level < 1) {
		cprintf(cpu, "CPU level %d not supported ...\n",
			cpu->arch.level);
		return 0;
	}
	cprintf(cpu, "CPU level %d ok\n", cpu->arch.level);

	/* Look for manufacturer identifier */
	memcpy(ident    , &ebx, 4);
	memcpy(ident + 4, &edx, 4);
	memcpy(ident + 8, &ecx, 4);
	ident[sizeof(ident) - 1 /* 12 */] = 0; /* String terminator ;-) */
	cprintf(cpu, "Manufacturer string is '%s'\n", ident);

	/* Now that we have the CPU identifier let us find the descriptor */
	dprintf("Scanning %d CPU descriptors\n",
		sizeof(cpu_descriptors) / sizeof(cpu_descriptor_t));

	descriptor = NULL;
	if (sizeof(cpu_descriptors) != 0) {
		size_t i;

		i = 0;
		for (;
		     i  < (sizeof(cpu_descriptors) / sizeof(cpu_descriptor_t));
		     i++) {

			if ( cpu_descriptors[i].ident == NULL ||
			     !strcmp(cpu_descriptors[i].ident, ident)) {
				dprintf("Matching CPU descriptor found!\n");
				descriptor = &cpu_descriptors[i];
				break;
			} else {
				dprintf("CPU description '%s' doesn't match\n",
					descriptor->ident);
			}
		}
	} else {
		cprintf(cpu, "No configured descriptors found\n");
		return 0;
	}

	assert(descriptor != NULL);

	dprintf("CPU descriptor is '%s'/'%s'\n",
		(descriptor->ident  ? descriptor->ident  : "Unknown"),
		(descriptor->vendor ? descriptor->vendor : "Unknown"));

	/*
	 * NOTE:
	 *    descriptor is now pointing to a specific CPU
	 */
	cprintf(cpu,"Installing %s CPU specific ops\n",	descriptor->vendor);
	cpus->arch.ops = &(descriptor->ops);

	dprintf("Checking required CPU ops\n");

	assert(cpu->arch.ops);
	assert(cpu->arch.ops->infos);
	assert(cpu->arch.ops->cpu_init);
	assert(cpu->arch.ops->cpu_fini);

	if (!cpu->arch.ops->infos(&cpu->arch)) {
		cprintf(cpu, "Cannot get CPU infos\n");
		return 0;
	}
	cprintf(cpu, "family %d, model %d, stepping %d\n",
		cpu->arch.infos.family,
		cpu->arch.infos.model,
		cpu->arch.infos.stepping);

#if 0
	cprintf(cpu,
		"CPU%d %08x %08x %08x %08x %08x %08x %08x\n",
		cpu->arch.infos.features[0],
		cpu->arch.infos.features[1],
		cpu->arch.infos.features[2],
		cpu->arch.infos.features[3],
		cpu->arch.infos.features[4],
		cpu->arch.infos.features[5],
		cpu->arch.infos.features[6],
		cpu->arch.infos.features[7]);
#endif

	cprintf(cpu,
		"caps = "
		"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		(cpu_has_fpu(cpu)            ? "fpu "            : ""),
		(cpu_has_vme(cpu)            ? "vme "            : ""),
		(cpu_has_de(cpu)             ? "de "             : ""),
		(cpu_has_pse(cpu)            ? "pse "            : ""),
		(cpu_has_tsc(cpu)            ? "tsc "            : ""),
		(cpu_has_pae(cpu)            ? "pae "            : ""),
		(cpu_has_pge(cpu)            ? "pge "            : ""),
		(cpu_has_apic(cpu)           ? "apic "           : ""),
		(cpu_has_sep(cpu)            ? "sep "            : ""),
		(cpu_has_mtrr(cpu)           ? "mtrr "           : ""),
		(cpu_has_mmx(cpu)            ? "mmx "            : ""),
		(cpu_has_fxsr(cpu)           ? "fxsr "           : ""),
		(cpu_has_xmm(cpu)            ? "xmm "            : ""),
		(cpu_has_xmm2(cpu)           ? "xmm2 "           : ""),
		(cpu_has_xmm3(cpu)           ? "xmm3 "           : ""),
		(cpu_has_ht(cpu)             ? "ht "             : ""),
		(cpu_has_mp(cpu)             ? "mp "             : ""),
		(cpu_has_nx(cpu)             ? "nx "             : ""),
		(cpu_has_k6_mtrr(cpu)        ? "k6-mtrr "        : ""),
		(cpu_has_cyrix_arr(cpu)      ? "cyrix-arr "      : ""),
		(cpu_has_centaur_mcr(cpu)    ? "centaur-mcr "    : ""),
		(cpu_has_xstore(cpu)         ? "xstore "         : ""),
		(cpu_has_xstore_enabled(cpu) ? "xstore-enabled " : ""),
		(cpu_has_xcrypt(cpu)         ? "xcrypt "         : ""),
		(cpu_has_xcrypt_enabled(cpu) ? "xcrypt-enabled " : ""));

	if (!cpu->arch.ops->cpu_init(&cpu->arch)) {
		cprintf(cpu, "Cannot initialize CPU\n");
		return 0;
	}

	/* Looking for cache ... */
	cpu->arch.caches.trace.size   = 0;
	cpu->arch.caches.l1i.size     = 0;
	cpu->arch.caches.l1d.size     = 0;
	cpu->arch.caches.l2.size      = 0;
	cpu->arch.caches.l3.size      = 0;

	/* Call the cache initialization */
	if (cpu->arch.ops->cache_init) {
		if (!cpu->arch.ops->cache_init(&cpu->arch)) {
			/*
			 * NOTE:
			 *     The cache initialization could be unsuccessful
			 *     (e.g.: If the CPU level is too low) ... but it
			 *     is not a real problem. We shouldn't return 0
			 *     here.
			 */
			cprintf(cpu, "Cannot initialize cache\n");
		}
	}

	cprintf(cpu,
		"Cache: Trace %dK, L1-I %dK, L1-D %dK, L2 %dK, L3 %dK\n",
		cpu->arch.caches.trace.size,
		cpu->arch.caches.l1i.size, cpu->arch.caches.l1d.size,
		cpu->arch.caches.l2.size,
		cpu->arch.caches.l3.size);

	return 1;
}

/* Per CPU finalizer */
void cpu_fini(int indx)
{
	cpu_t*       cpu;

	cpu = &cpus[indx];

	assert(cpu->arch.ops);

	/* First call the cache finalizer */
	if (cpu->arch.ops->cache_fini) {
		cpu->arch.ops->cache_fini(&cpu->arch);
	}

	/* First call the cpu finalizer */
	if (cpu->arch.ops->cpu_fini) {
		cpu->arch.ops->cpu_fini(&cpu->arch);
	}
}



/* All CPUs initializer */
int cpus_init(void)
{
	unsigned int i, o;

	/* We need cpuid() ... */
	if (!have_cpuid()) {
		dprintf("Sorry, this architecture has no cpuid instruction\n");
		return 0;
	}

	/* feeewh, we can go on ;-) */
	o = 0;
	for (i = 0; i < CONFIG_MAX_CPU_COUNT; i++) {
		/* Put the CPU offline first */
		cpus[i].online = 0;

		/* Initialize it */
		if (!cpu_init(i)) {
			dprintf("Cannot initialize CPU%d\n", i);
			continue;
		}

		/* All is ok, set the CPU online */
		cpus[i].online = 1;
		o++;
	}

	dprintf("%d/%d CPUs initialized successfully\n", o, i);

	/* Enable write protection from kernel code */
	cr0_set(cr0_get() | CR0_WP);

	/*
	 * NOTE:
	 *     Setup eflags register:
	 *         Interrupt disable, clear direction,
	 *         clear nested task, I/O privilege 0
	 */
	eflags_set(eflags_get() & ~(EFLAGS_IF |
				    EFLAGS_DF |
				    EFLAGS_NT |
				    EFLAGS_IOPL));

	/* Is (quite) ok if there's (at least) a CPU online */
	return (o ? 1 : 0);
}

/* All CPUs finalizer */
void cpus_fini(void)
{
	unsigned int i, o;

	o = 0;
	for (i = 0; i < CONFIG_MAX_CPU_COUNT; i++) {
		/* Finalize only the online CPUs */
		if (cpus[i].online) {
			cpu_fini(i);
			cpus[i].online = 0;
		}
	}

	dprintf("%d/%d CPUs finalized successfully\n", o, i);
}

/*
 * NOTE:
 *     Architecture specific calls
 */
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
