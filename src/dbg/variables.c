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
#include "archs/linker.h"
#include "dbg/debug.h"
#include "dbg/debugger.h"

#if CONFIG_DEBUGGER

#define BANNER           "dbg: "

#if CONFIG_DEBUGGER_DEBUG
#define dprintf(F,A...)  printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

dbg_variable_t* variables = NULL;

static dbg_variable_t* variable_lookup(const char* string)
{
        dbg_variable_t* temp;

        assert(string);

        dprintf("Lookup for variable '%s'\n", string);

        temp = variables;
        while (temp) {
                if (!strcmp(temp->name, string)) {
                        dprintf("Found!\n");
                        return temp;
                }

                temp = temp->next;
        }

        dprintf("Not found\n");

        return NULL;
}

/*
 * XXX FIXME:
 *     We should replace some asserts with proper checks.
 */

int dbg_variable_add(dbg_variable_t* variable)
{
        assert(variable);

        /* We need the variable name */
        assert(variable->name);

        /* We don't want to add a variable with an already present name */
        assert(!variable_lookup(variable->name));

        /* Ok, we have the variable descriptor, some consistency checks now  */
        assert(variable->range.min < variable->range.max);

        /* We should be safe now ... */
        variable->next = variables;
        variables      = variable;

        dprintf("Variable `%s` added successfully\n", variable->name);

        return 1;
}

int dbg_variable_remove(dbg_variable_t* variable)
{
        dbg_variable_t* prev;
        dbg_variable_t* curr;

        assert(variable);
        assert(variable->name);

        /* Remove a variable from the debugger variables list */

        prev = NULL;
        curr = variables;
        while (curr) {
                if (curr->name == variable->name) {
                        /* Found it, now simply remove it from the list */
                        if (prev) {
                                /* Not head element */
                                prev->next = curr->next;
                        } else {
                                /* Head element */
                                variables = curr->next;
                        }

                        /* Variable successfully removed */
                        return 1;
                }

                /* Not found, next please ... */
                prev = curr;
                curr = curr->next;
        }

        /* Huh ... variable not found ... */
        dprintf("Variable not found, removal unsuccessful\n");

        return 0;
}

const dbg_variable_t* dbg_variable_lookup(const char* string)
{
        if (!string) {
                return NULL;
        }

        return variable_lookup(string);
}

dbg_result_t dbg_variable_get(const char* string, int* value)
{
        const dbg_variable_t* tmp;

        assert(string);
        assert(value);

        tmp = variable_lookup(string);
        if (!tmp) {
                /* Variable not found */
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        /* Variable value should be always in-range */
        assert((*value >= tmp->range.min) || (*value <= tmp->range.max));

        *value = tmp->value;
        return DBG_RESULT_OK;
}

dbg_result_t dbg_variable_set(const char* string, int value)
{
        dbg_variable_t* tmp;

        assert(string);

        tmp = variable_lookup(string);
        if (!tmp) {
                /* Variable not found */
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        if (value < tmp->range.min) {
                return DBG_RESULT_ERROR_VALUE_TOOLOW;
        } else if (value > tmp->range.max) {
                return DBG_RESULT_ERROR_VALUE_TOOHIGH;
        }

        tmp->value = value;
        return DBG_RESULT_OK;
}

int dbg_variables_config(dbg_variable_t** start,
                         dbg_variable_t** stop)
{
        dbg_variable_t** tmp;

        assert(start);
        assert(stop);
        assert(start < stop);

        dprintf("Configuring debugger variables (start = 0x%x, stop = 0x%x)\n",
                start, stop);

        /* Scan the debugger variables linking them togheter */
        tmp = start;
        while (tmp < stop) {
                assert(tmp);

                dprintf("Variable (0x%x)\n", *tmp);

                assert((*tmp)->name);

                dprintf("  Name          = '%s'\n",
                        (*tmp)->name);
#if 0
                dprintf("  Help          = '%s'\n",
                        (*tmp)->help);
                dprintf("  Min/Max/Value = %d/%d/%d\n",
                        (*tmp)->range.min,
                        (*tmp)->range.max,
                        (*tmp)->value);
                dprintf("  Type          = 0x%x\n",
                        (*tmp)->type);
                dprintf("  Callbacks     = 0x%x/0x%x/0x%x/0x%x\n",
                        (*tmp)->actions.on_enter,
                        (*tmp)->actions.on_set,
                        (*tmp)->actions.on_get,
                        (*tmp)->actions.on_leave);
#endif

                if (!dbg_variable_add(*tmp)) {
                        dprintf("Cannot add debugger variable '%s'",
                                (*tmp)->name);
                        return 0;
                }

                tmp++;
        }

        return 1;
}

int dbg_variables_unconfig(dbg_variable_t** start,
                           dbg_variable_t** stop)
{
        dbg_variable_t** tmp;

        assert(start);
        assert(stop);
        assert(start < stop);

        tmp = start;
        while (tmp < stop) {
                assert(tmp);
                assert((*tmp)->name);

                if (!dbg_variable_remove(*tmp)) {
                        dprintf("Cannot remove debugger variable '%s'\n",
                                (*tmp)->name);
                        return 0;
                }

                tmp++;
        }

        return 1;
}

int dbg_variables_init(void)
{
        assert(!variables);

        dprintf("Initializing debugger variables\n");

        if (!dbg_variables_config(&__DBGVARS_LIST__, &__DBGVARS_END__)) {
                dprintf("Cannot initialize kernel debugger variables\n");
                return 0;
        }

        return 1;
}

void dbg_variables_fini(void)
{
        assert(variables);

        dprintf("Finalizing debugger variables\n");

        if (!dbg_variables_unconfig(&__DBGVARS_LIST__, &__DBGVARS_END__)) {
                dprintf("Cannot finalize kernel debugger variables\n");
        }
}

#endif /* CONFIG_DEBUGGER */
