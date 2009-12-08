/* -*- c -*- */

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
#include "dbg/debug.h"

/* For atomicity, we only need to lock the bus on SMP systems */
#if CONFIG_MAX_CPU_COUNT > 1
#define LOCK "lock;"
#else
#define LOCK ""
#endif

int arch_atomic_add(int* pointer,
                    int  delta)
{
        int r;

        __asm__ volatile (LOCK "xaddl %%eax, (%%ebx)"
                          :"=a" (r)
                          :"0" (delta), "b" ((int) pointer));

        return r;
}

int arch_atomic_sub(int* pointer,
                    int  delta)
{
        int r;

        __asm__ volatile (LOCK "xaddl %%eax, (%%ebx)"
                          :"=a" (r)
                          :"0" (-delta), "b" ((int) pointer));

        return r;
}

int arch_atomic_exchange(int* pointer,
                         int  new_value)
{
        int old_value;

        __asm__ volatile ("xchgl (%1), %0"
                          : "=r" (old_value)
                          : "0"  (new_value), "r" ((int) pointer)
                          : "memory");

        return old_value;
}

int arch_atomic_test_and_set(int* pointer)
{
        int r;

        __asm__ volatile (LOCK "bts $0, (%1)\n"
                          "xorl %%eax, %%eax\n"
                          "setc %%al\n"
                          : "=a" (r)
                          : "r" ((int) pointer));

        return r;
}

int arch_atomic_set(int* pointer,
                    int  value)
{
        return arch_atomic_exchange(pointer, value);
}
