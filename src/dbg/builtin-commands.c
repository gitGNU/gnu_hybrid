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
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "libc/string.h"
#include "libc/ctype.h"
#include "libbfd/bfd.h"
#include "archs/boot/option.h"
#include "libs/debug.h"
#include "dbg/backtrace.h"
#include "dbg/debugger.h"

#if CONFIG_DEBUGGER

#define BANNER           "dbg: "

#if CONFIG_DEBUGGER_DEBUG
#define dprintf(F,A...)  printf(BANNER F,##A)
#define dputs(S)         puts(S)
#else
#define dprintf(F,A...)
#define dputs(S)
#endif

extern dbg_command_t*   commands;
extern dbg_variable_t*  variables;

static dbg_result_t command_help_on_execute(FILE* stream,
                                            int   argc,
                                            char* argv[])
{
        const dbg_command_t* temp;

        assert(stream);
        assert(argc >= 0);

        assert(commands);

        if (argc > 1) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        if (argc == 0) {
                /* Global help requested */

                int    ret;

                size_t max_name;
                char   sformat[] = "  %c-%02d%c    %c%c\n";
                char   format[]  = "                     ";

                fprintf(stream, "Available commands:\n");
                fprintf(stream, "\n");

                /* Look for the maximum command name length */
                max_name  = 0;
                temp      = commands;
                while (temp) {
                        assert(temp->name);

                        if (max_name < strlen(temp->name)) {
                                max_name = strlen(temp->name);
                        }

                        temp = temp->next;
                }
                temp = NULL; /* Do not access temp anymore */

                dprintf("max_name=%d\n", max_name);

                /* Some sane defaults */
                assert(max_name  < 100);

                dputs(sformat);

                /* Build the format string */
                ret = snprintf(format, strlen(format), sformat,
                               '%', max_name, 's', '%', 's');
                assert(ret >= 0);

                dputs(format);

                /* Dump the commands help */
                temp = commands;
                while (temp) {
                        assert(temp->name);

                        fprintf(stream, format,
                                temp->name,
                                temp->help.short_form ?
                                temp->help.short_form : "");

                        temp = temp->next;
                }

                fprintf(stream, "\n");
                fprintf(stream,
                        "Use 'help <command>' in order to get more infos\n");

                return DBG_RESULT_OK;
        } else if (argc == 1) {
                /* Specific help requested */

                assert(argv[0]);

                temp = dbg_command_lookup(argv[0]);
                if (!temp) {
                        fprintf(stream, "Unknown command '%s'\n", argv[0]);
                        return DBG_RESULT_ERROR;
                }

                if (!(temp->help.long_form)) {
                        fprintf(stream,
                                "Command '%s' has no long help\n", argv[0]);
                        return DBG_RESULT_OK;
                }

                fprintf(stream, "Command '%s':\n", argv[0]);
                fprintf(stream, "\n");

                /*
                 * XXX FIXME:
                 *     We should add a proper formatting here ...
                 */

                fprintf(stream, "    %s\n", temp->help.long_form);
                fprintf(stream, "\n");

                return DBG_RESULT_OK;
        }

        bug();

        return DBG_RESULT_ERROR;
}

static dbg_result_t command_help_on_enter(FILE* stream)
{
        assert(stream);

        fprintf(stream, "Use 'help' to get commands list\n");

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(help,
                    "Dumps a brief help",
                    NULL,
                    command_help_on_enter,
                    command_help_on_execute,
                    NULL);

#if CONFIG_DEBUGGER_HISTORY
static dbg_result_t command_history_on_execute(FILE* stream,
                                               int   argc,
                                               char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc > 1) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        if (argc == 0) {
                int count;

                count = dbg_history_count();
                if (count != 0) {
                        int i;

                        fprintf(stream, "History:\n");
                        fprintf(stream, "\n");

                        for (i = 0; i < count; i++) {
                                const char* s;

                                s = dbg_history_retrieve(i);

                                /*
                                 * NOTE:
                                 *     The history line must be not empty
                                 *     because it should have been checked
                                 *     previously, the history gave us a
                                 *     count so we should be safe ...
                                 */
                                assert(s);

                                fprintf(stream, "  %d %s\n", i, s);
                        }

                        fprintf(stream, "\n");
                } else {
                        fprintf(stream, "History is empty\n");
                }

                return DBG_RESULT_OK;

        } else if (argc == 1) {

                if (strcmp(argv[0], "clear")) {
                        return DBG_RESULT_ERROR_WRONG_PARAMETERS;
                }

                dbg_history_clear();

                return DBG_RESULT_OK;
        }

        bug();
        return DBG_RESULT_ERROR;
}

DBG_COMMAND_DECLARE(history,
                    "Dumps command history",
                    "Dumps the command history, pass a number in order to "
                    "retrieve a particular history record. The history buffer "
                    "is not resizable",
                    NULL,
                    command_history_on_execute,
                    NULL);

static dbg_result_t command_recall_on_execute(FILE* stream,
                                              int   argc,
                                              char* argv[])
{
        int         i;
        const char* s;

        assert(stream);
        assert(argc >= 0);

        if (argc < 1) {
                return DBG_RESULT_ERROR_MISSING_PARAMETERS;
        }

        if (argc > 1) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        assert(argv[0]);

        /* Code needed ... */

        i = atoi(argv[0]);
        s = dbg_history_retrieve(i);
        if (!s) {
                /* The user may gave incorrect indexes */
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        dprintf("History line at index %d is '%s'\n", i, s);

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(recall,
                    "Recalls a command from history",
                    "Recalls an item from history. Pass the item number as "
                    "parameter",
                    NULL,
                    command_recall_on_execute,
                    NULL);

#endif /* CONFIG_DEBUGGER_HISTORY */

static dbg_result_t command_leave_on_execute(FILE* stream,
                                             int   argc,
                                             char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        unused_argument(argv);

        return DBG_RESULT_EXIT;
}

DBG_COMMAND_DECLARE(quit,
                    "Exit the debugger",
                    NULL,
                    NULL,
                    command_leave_on_execute,
                    NULL);

DBG_COMMAND_DECLARE(exit,
                    "Exit the debugger",
                    NULL,
                    NULL,
                    command_leave_on_execute,
                    NULL);

DBG_COMMAND_DECLARE(bye,
                    "Exit the debugger",
                    NULL,
                    NULL,
                    command_leave_on_execute,
                    NULL);

static dbg_result_t command_version_on_execute(FILE* stream,
                                               int   argc,
                                               char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

#if defined(HAVE___DATE__) && defined(HAVE___TIME__)
        fprintf(stream, "%s %s (compiled on %s %s)\n",
                PACKAGE_NAME, PACKAGE_VERSION, __DATE__, __TIME__);
#else
        fprintf(stream, "%s %s\n",
                PACKAGE_NAME, PACKAGE_VERSION);
#endif

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(version,
                    "Dumps kernel version",
                    NULL,
                    NULL,
                    command_version_on_execute,
                    NULL);

static dbg_result_t command_breakpoint_on_execute(FILE* stream,
                                                  int   argc,
                                                  char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (!(argc >= 0 && argc <= 1)) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        assert(argv);

        if (argc == 0) {

                /* Show the breakpoints */
                fprintf(stream, "Breakpoints:\n");

                missing();

                return DBG_RESULT_OK;

        } else if (argc == 1) {
                uint_t breakpoint;

                /* Set the breakpoint */
                assert(argv[0]);

                breakpoint = 0;

                fprintf(stream, "Breakpoint set to 0x%08x\n", breakpoint);

                missing();
                return DBG_RESULT_OK;

        }

        bug();

        return DBG_RESULT_ERROR; /* Stop the compiler barf */
}

DBG_COMMAND_DECLARE(breakpoint,
                    "Set  a breakpoint",
                    NULL,
                    NULL,
                    command_breakpoint_on_execute,
                    NULL);

static dbg_result_t command_step_on_execute(FILE* stream,
                                            int   argc,
                                            char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Step:\n");

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(step,
                    "Step instructions",
                    NULL,
                    NULL,
                    command_step_on_execute,
                    NULL);

static dbg_result_t command_continue_on_execute(FILE* stream,
                                                int   argc,
                                                char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Continuing program execution ...\n");

        missing();

        return DBG_RESULT_EXIT;
}

DBG_COMMAND_DECLARE(continue,
                    "Continue execution",
                    NULL,
                    NULL,
                    command_continue_on_execute,
                    NULL);

static dbg_result_t command_disassemble_on_execute(FILE* stream,
                                                   int   argc,
                                                   char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc < 2) {
                return DBG_RESULT_ERROR_MISSING_PARAMETERS;
        }
        if (argc > 2) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Disassemble:\n");

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(disassemble,
                    "Disassemble",
                    NULL,
                    NULL,
                    command_disassemble_on_execute,
                    NULL);

static dbg_result_t command_frame_on_execute(FILE* stream,
                                             int   argc,
                                             char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Frame:\n");

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(frame,
                    "Show a stack frame",
                    NULL,
                    NULL,
                    command_frame_on_execute,
                    NULL);

static dbg_result_t command_set_on_execute(FILE* stream,
                                           int   argc,
                                           char* argv[])
{
        const dbg_variable_t* var;
        int                   value;
        dbg_result_t          ret;

        assert(stream);
        assert(argc >= 0);

        if (argc != 2) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        assert(argv);
        assert(argv[0]);
        assert(argv[1]);

        var = dbg_variable_lookup(argv[0]);
        if (!var) {
                /* Variable not found */
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        value = atoi(argv[1]);
        if (var->type == DBG_VAR_RW) {
                if (var->actions.on_set) {
                        /* User action for set */
                        ret = var->actions.on_set(stream, value);
                } else {
                        /* Default action for set */
                        ret = dbg_variable_set(argv[0], value);
                }
        } else {
                /* Variable is read-only */
                fprintf(stream, "Variable '%s' is not writable\n", argv[0]);
                ret = DBG_RESULT_ERROR;
        }

        return ret;
}

DBG_COMMAND_DECLARE(set,
                    "Set a variable",
                    NULL,
                    NULL,
                    command_set_on_execute,
                    NULL);

static dbg_result_t command_get_on_execute(FILE* stream,
                                           int   argc,
                                           char* argv[])
{
        const dbg_variable_t* var;
        int                   value;
        dbg_result_t          ret;

        assert(stream);
        assert(argc >= 0);

        if (argc != 1) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        assert(argv);
        assert(argv[0]);

        var = dbg_variable_lookup(argv[0]);
        if (!var) {
                /* Variable not found */
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        value = 0;
        ret   = DBG_RESULT_ERROR;
        if ((var->type == DBG_VAR_RO) || (var->type == DBG_VAR_RW)) {
                if (var->actions.on_get) {
                        /* User action for get */
                        ret = var->actions.on_get(stream, &value);
                } else {
                        /* Default action for get */
                        ret = dbg_variable_get(argv[0], &value);
                }
        } else {
                /* A variable should always be readable ... */
                bug();
        }

        if (ret == DBG_RESULT_OK) {
                fprintf(stream, "Variable %s = %d\n", var->name, value);
        }

        return ret;
}

DBG_COMMAND_DECLARE(get,
                    "Get a variable",
                    NULL,
                    NULL,
                    command_get_on_execute,
                    NULL);

static dbg_result_t command_cls_on_execute(FILE* stream,
                                           int   argc,
                                           char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_WRONG_PARAMETERS;
        }

        unused_argument(argv);

        missing();

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(cls,
                    "Clear the screen",
                    NULL,
                    NULL,
                    command_cls_on_execute,
                    NULL);

static dbg_result_t command_echo_on_execute(FILE* stream,
                                            int   argc,
                                            char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc > 1) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        assert(argv);

        if (argc < 1) {

                fprintf(stream, "\n");
                return DBG_RESULT_OK;

        } else if (argc == 1) {

                assert(argv[0]);
                fprintf(stream, "%s\n", argv[0]);
                return DBG_RESULT_OK;

        }

        bug();

        return DBG_RESULT_ERROR; /* Stop the compiler barf */
}

DBG_COMMAND_DECLARE(echo,
                    "Echo the passed string",
                    "Echo the passed string. The input string must be "
                    "protected using \" character",
                    NULL,
                    command_echo_on_execute,
                    NULL);

static dbg_result_t command_environment_on_execute(FILE* stream,
                                                   int   argc,
                                                   char* argv[])
{
        dbg_variable_t* temp;

        int             ret;

        size_t          max_name;
        char            sformat[] =
                "  %c-%02d%c    %c%c\n";
        char            format[]  =
                "                     "
                "                     ";

        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        fprintf(stream, "Available variables:\n");
        fprintf(stream, "\n");

        /* Look for the maximum command name length */
        max_name  = 0;
        temp = variables;
        while (temp) {
                assert(temp->name);

                if (max_name < strlen(temp->name)) {
                        max_name = strlen(temp->name);
                }

                temp = temp->next;
        }
        temp = NULL; /* Do not access temp */

        dprintf("max_name=%d\n", max_name);

        /* Some sane defaults */
        assert(max_name  < 100);

        dputs(sformat);

        /* Build the format string */
        ret = snprintf(format, strlen(format), sformat,
                       '%', max_name,  's', '%', 's');
        assert(ret >= 0);

        dputs(format);

        /* Dump the variables environment */
        temp = variables;
        while (temp) {
                assert(temp->name);

                fprintf(stream, format,
                        temp->name,
                        temp->help ? temp->help : "");

                temp = temp->next;
        }

        fprintf(stream, "\n");

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(environment,
                    "Environment dump",
                    "Shows the environment variables",
                    NULL,
                    command_environment_on_execute,
                    NULL);

#endif /* CONFIG_DEBUGGER */
