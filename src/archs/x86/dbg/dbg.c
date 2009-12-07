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
#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/stddef.h"
#include "libc/ctype.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

#if CONFIG_DEBUG

extern int  rs232_init(void);
extern int  rs232_getchar(void);
extern int  rs232_putchar(int c);
extern void rs232_fini(void);

extern int  kbd_init(void);
extern int  kbd_getchar(void);
extern void kbd_fini(void);

extern int  vga_init(void);
extern int  vga_putchar(int c);
extern void vga_fini(void);

extern int  bochs_init(void);
extern int  bochs_putchar(int c);
extern void bochs_fini(void);

/* getchar() is from the AT keyboard or RS232 */
int arch_dbg_getchar(void)
{
#if CONFIG_HOSTED_DEBUGGER
        return kbd_getchar();
#endif
#if CONFIG_RS232_DEBUGGER
        return rs232_getchar();
#endif
#if CONFIG_BOCHS_DEBUGGER
        /* XXX FIXME: Are we sure ? */
        return EOF;
#endif
}

/* putchar() sends output to the VGA or RS232 (tees output to bochs console) */
int arch_dbg_putchar(int c)
{
        int r;

#if CONFIG_BOCHS_DEBUGGER
        /* Tee the output to the bochs console */
        (void) bochs_putchar(c);
#endif

        r = EOF;

#if CONFIG_HOSTED_DEBUGGER
        r = vga_putchar(c);
#endif
#if CONFIG_RS232_DEBUGGER
        r = rs232_putchar(c);
#endif

        return r;
}

int arch_dbg_init(void)
{
#if CONFIG_BOCHS_DEBUGGER
        (void) bochs_init();
#endif
#if CONFIG_HOSTED_DEBUGGER
        if (!vga_init()) {
                /*
                 * NOTE:
                 *     We don't have output ... output is prioritary over
                 *     input, so this condition is an hard-error. We will
                 *     return 0 in order to notify back the problem
                 */
                return 0;
        }
        if (!kbd_init()) {
                /*
                 * NOTE:
                 *     The keyboard is not correctly initialized, we should
                 *     return 0 and stopping other activities ... but we have
                 *     the output available so we can continue ...
                 *
                 */
        }
#endif
#if CONFIG_RS232_DEBUGGER
        if (!rs232_init()) {
                return 0;
        }
#endif
        return 1;
}

void arch_dbg_fini(void)
{
#if CONFIG_RS232_DEBUGGER
        rs232_fini();
#endif
#if CONFIG_HOSTED_DEBUGGER
        vga_fini();
        kbd_fini();
#endif
#if CONFIG_BOCHS_DEBUGGER
        bochs_fini();
#endif
}
#endif /* CONFIG_DEBUG */
