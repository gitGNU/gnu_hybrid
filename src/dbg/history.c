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
#include "libc/stddef.h"
#include "libc/string.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

#if CONFIG_DEBUGGER_HISTORY

#define BANNER           "dbg: "

#if CONFIG_DEBUGGER_DEBUG
#define dprintf(F,A...)  printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static int initialized = 0;

static struct {
        int  index;
        int  count;
        char lines[CONFIG_DEBUGGER_HISTORY_LINES][CONFIG_DEBUGGER_LINE_LENGTH];
} history;

#define CHECK_BOUNDS                                            \
        __BEGIN_MACRO                                           \
        assert(0 <= history.count);                             \
        assert(history.count <= CONFIG_DEBUGGER_HISTORY_LINES); \
        assert(history.index >= 0);                             \
        __END_MACRO

void dbg_history_clear(void)
{
        assert(initialized);
        CHECK_BOUNDS;

        dprintf("History cleared\n");

        history.index = 0;
        history.count = 0;
        memset(history.lines, 0, sizeof(history.lines));
}

int dbg_history_init(void)
{
        assert(!initialized);

        initialized = 1;
        dbg_history_clear();

        return 1;
}

int dbg_history_count(void)
{
        assert(initialized);
        CHECK_BOUNDS;

        return history.count;
}

int dbg_history_add(const char* string)
{
        int i;
        int l;

        assert(initialized);
        assert(string);
        CHECK_BOUNDS;

        l = strlen(string);
        if (l > CONFIG_DEBUGGER_LINE_LENGTH) {
                dprintf("String length exceed maximum allowed\n");
                return 0;
        }

        i = (history.index % CONFIG_DEBUGGER_HISTORY_LINES);
        strcpy(&history.lines[i][0], string);

        history.index++;

        /* Update the elements count */
        history.count++;
        if (history.count >= CONFIG_DEBUGGER_HISTORY_LINES) {
                /* Do not cross the maximum allowed index */
                history.count = CONFIG_DEBUGGER_HISTORY_LINES;
        }

        dprintf("Total entries: %d\n", dbg_history_count());

        return 1;
}

const char* dbg_history_retrieve(int indx)
{
        int i;

        assert(initialized);
        CHECK_BOUNDS;

        if (history.count == 0) {
                dprintf("History is empty\n");
                return NULL;
        }

        if ((indx < 0) || (indx >= history.count)) {
                dprintf("Index %d is out of bounds\n", indx);
                return NULL;
        }

        /*
         * NOTE:
         *     We need to translate external index (0 .. (history.count - 1))
         *     into the internal one
         */
        i = ((history.index - 1) - (history.count - 1) + indx) %
                CONFIG_DEBUGGER_HISTORY_LINES;

        dprintf("External index %d translated into Internal index %d\n",
                indx, i);

        assert(i >= 0);
        assert(i < CONFIG_DEBUGGER_HISTORY_LINES);

        return history.lines[i];
}

void dbg_history_fini(void)
{
        assert(initialized);
        CHECK_BOUNDS;

        initialized = 0;
}

#endif /* CONFIG_DEBUGGER_HISTORY */
