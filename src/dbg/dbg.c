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
#include "libc/string.h"
#include "libc/ctype.h"
#include "core/interrupt.h"
#include "dbg/debug.h"
#include "dbg/debugger.h"

#if CONFIG_DEBUGGER

#define BANNER           "dbg: "

#if CONFIG_DEBUGGER_DEBUG
#define dprintf(F,A...)  printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static char*           prompt      = "> ";
extern dbg_command_t*  commands;           /* Commands head pointer */
extern dbg_variable_t* variables;          /* Variables head pointer */
static int             initialized = 0;

#define stddbg_out stdout
#define stddbg_in  stdin

extern int  dbg_commands_init(void);
extern void dbg_commands_fini(void);
extern int  dbg_variables_init(void);
extern void dbg_variables_fini(void);

int dbg_init(void)
{
        assert(!initialized);

        dprintf("Initializing\n");

        if (!dbg_commands_init()) {
                panic("Cannot initialize debugger commands");
        }

        if (!dbg_variables_init()) {
                panic("Cannot initialize debugger variables");
        }

#if CONFIG_DEBUGGER_HISTORY
        if (!dbg_history_init()) {
                panic("Cannot initialize debugger history");
        }
#endif

        initialized = 1;

        return 1;
}

/*
 * NOTE:
 *     Our getline() is not like the standard getline() function:
 *
 *         ssize_t getline(char **lineptr, size_t *n, FILE *stream)
 *
 */
static ssize_t getline(char*  buffer,
                       size_t length,
                       FILE*  stream_in,
                       FILE*  stream_out)
{
        size_t i;
        int complete;
        int sensitiveness;

        assert(buffer);
        assert(stream_in);
        assert(stream_out);

        fprintf(stream_out, prompt);

        sensitiveness = 1;
        (void) dbg_variable_get("case", &sensitiveness);

        complete      = 0;
        for (i = 0; i < length && !complete;) {
                char c;

                /*
                 * NOTE:
                 *     Here we have stream_out for output and stream_in
                 *     for input ...
                 */

                /* Get it */
                c = fgetc(stream_in);
                if (c == EOF) {
                        /* Huh ... we got problems in the stream ... */
                        return 0;
                }

                /* Handle control/unwanted characters */
                if (c == '\t' || c == '\r') {
                        continue;
                }

                if (c == '\b') {
                        /* Handle backspace ... */
                        if (i > 0) {
                                /* Only if we are not at the beginning */
                                i--;

                                /* Echo it */
                                fputc(c, stream_out);
                        }
                        continue;
                }

                /* Handle special cases */
                if (c == '\n') {
                        /* The line is complete ... */
                        complete = 1;
                }

                /* Save it */

                /* XXX FIXME: Use strcasecmp() instead !!! This harm history */
                buffer[i] = sensitiveness ? c : tolower(c);

                /* Echo it, if it is printable ... */
                if (isprint(c)) {
                        fputc(c, stream_out);
                }

                i++;
        }

        /* Always place the terminator */
        buffer[i] = 0;

        fputc('\n', stream_out); /* New line :-) */

        return i;
}

static int tokenize(char* buffer, int* argc, char** argv, int max)
{
        int   i;        /* argv index */
        int   j;        /* buffer index */
        int   l;        /* buffer length */
        char* escapes;  /* Escaping character */
        int   protect;  /* Foolish FSM state ... */

        assert(buffer);
        assert(argc);
        assert(argv);

        l = strlen(buffer);
        dprintf("Start tokenizing (string length = %d)\n", l);

        /*
         * NOTE:
         *     Simplify: trasform all spaces to nulls && remove control
         *     chars. Handle protected strings (") as a whole.
         */
        j       = 0;
        protect = 0;
        escapes  = "\"\'";
        for (;j < l;) {
                assert((protect == 0) || (protect == 1));

                /*
                 * NOTE:
                 *     Skip the chars while we are inside an escaped string
                 *     but remove the escape marker as well.
                 */
                if (index(escapes, buffer[j])) {
                        if (!protect) {
                                /* Turn protection on! */
                                protect   = 1;
                        } else {
                                /* Turn protection off */
                                protect   = 0;
                        }

                        buffer[j] = 0; /* Remove escape marker */
                        j++;
                        continue;
                }

                if (protect) {
                        /* Skip the chars while protecting them ... */
                        j++;
                        continue;
                }

                /* Normal case ... */
                if (isspace(buffer[j]) || iscntrl(buffer[j])) {
                        buffer[j] = 0;
                }

                j++;
        }

        /*
         * NOTE:
         *     At the end we must be in non-protected state, otherwise the
         *     string is not properly protected
         */
        if (protect) {
                return 0;
        }

        j = 0;
        for (i = 0; i < max; i++) {

                dprintf("Handling substring %d\n", i);

                /* Skip blanks until first (good) character */
                for (; j < l; j++) {
                        if (buffer[j] != 0) {
                                break;
                        }
                }

                if (j == l) {
                        /* Line is over ... */
                        dprintf("Line is over\n");
                        break;
                }

                /* Set argv */
                argv[i] = &buffer[j];

                /* Skip non-blanks until the first blank or End-Of-Line */
                for (; j < l; j++) {
                        if (buffer[j] == 0) {
                                break;
                        }
                }

                /* Place the terminator */
                buffer[j] = 0;

                dprintf("Substring %d is '%s'\n", i, argv[i]);
        }

        /* Set argc */
        *argc = i;

        /* Set the remaining argvs to NULL */
        for (; i < max; i++) {
                argv[i] = NULL;
        }

        dprintf("Stop tokenizing\n");

#if CONFIG_DEBUGGER
        dprintf("argv is:\n");
        j = 0;
        for (;j < max; j++) {
                if (argv[j]) {
                        dprintf("  argv[%02d] = '%s'\n", j, argv[j]);
                }
        }
#endif

        return 1;
}

static char dbg_buffer[CONFIG_DEBUGGER_LINE_LENGTH];

#define DBG_ARGC_MAX 20

/* Purify the input string removing useless characters */
static void purify(char* string)
{
        size_t len;

        assert(string);

        len = strlen(string);
        if (!len) {
                return;
        }

        /* Remove the \n marker at the end of the line (useless) */
        if (string[len - 1] == '\n') {
                string[len - 1] = '\0';
        }
}

/* Is the string really empty ? */
static int empty(char* string)
{
        size_t i;

        for (i = 0; i < strlen(string); i++) {
                if (!isspace(string[i])) {
                        return 0;
                }
        }

        return 1;
}

/* All the command execution related code must pass into this function */
static dbg_result_t execute(FILE* stream, char* string)
{
        int                  argc;
        char*                argv[DBG_ARGC_MAX];

        const dbg_command_t* command;
        dbg_result_t         retval;

        assert(stream);
        assert(string);

        purify(string);

        if (empty(string)) {
                /* String is empty */
                return DBG_RESULT_OK;
        }

        /* Put the current string into the history (even if it is garbage) */
#if CONFIG_DEBUGGER_HISTORY
        if (!dbg_history_add(string)) {
                fprintf(stream, "Cannot add new line into the history\n");
        }
#endif

        /* Tokenize it */
        if (!tokenize(string, &argc, argv, DBG_ARGC_MAX)) {
                fprintf(stream, "Wrong string format\n");
                return DBG_RESULT_ERROR;
        }

        assert(argc >= 0);

#if CONFIG_DEBUG
        {
                int i;

                dprintf("Tokenization completed\n");
                for (i = 0; i < argc; i++) {
                        dprintf("  argv[%02d]='%s'\n", i, argv[i]);
                }
        }
#endif

        /* String must be NOT empty (checked before) */
        assert((argc) || argv[0]);

        /* Find the command (with name 'argv[0]') */
        dprintf("Looking for command '%s'\n", argv[0]);
        command = dbg_command_lookup(argv[0]);
        if (!command) {
                fprintf(stream, "Unknown command '%s'\n", argv[0]);
                return DBG_RESULT_ERROR;
        }

        /* Check that the parameters we are trying to pass are correct */
        assert((argc - 1) >= 0);

        /* Then ... we've to execute it */
        dprintf("Trying to execute command\n");
        retval = command->actions.on_execute(stream, argc - 1, argv + 1);

        /* Fail if someone wrecked the command (name) */
        assert(command->name);

        switch (retval) {
        case DBG_RESULT_ERROR:
                fprintf(stream,
                        "Error executing command '%s'\n",
                        command->name);
                break;

        case DBG_RESULT_ERROR_VALUE_TOOLOW:
                fprintf(stream, "Value is too low\n");
                break;

        case DBG_RESULT_ERROR_VALUE_TOOHIGH:
                fprintf(stream, "Value is too high\n");
                break;

        case DBG_RESULT_ERROR_MISSING_PARAMETERS:
                /*
                 * Even if we checked it before ... we could get this
                 * return value because some commands gets a variable
                 * number of parameters ...
                 */
                fprintf(stream,
                        "Missing parameter(s) for command '%s'\n",
                        command->name);
                break;

        case DBG_RESULT_ERROR_TOOMANY_PARAMETERS:
                /* Same remark as the previous case ... */
                fprintf(stream,
                        "Too many parameter(s) for command '%s'\n",
                        command->name);
                break;

        case DBG_RESULT_ERROR_WRONG_PARAMETERS:
                fprintf(stream,
                        "Wrong parameter(s) for command '%s'\n",
                        command->name);
                break;

        case DBG_RESULT_ERROR_LINE_TOOLONG:
                fprintf(stream,
                        "Line too long!\n",
                        command->name);
                break;

        case DBG_RESULT_OK:
        case DBG_RESULT_EXIT:
                /* Ok */
                break;

        default:
                /* Huh unhandled error message, fix it! */
                bug();
                break;
        }

        return retval;
}

/*
 * NOTE:
 *     This is an entry-point for the debugger, this function is useful for
 *     injecting commands directly into the debugger
 */
int dbg_execute(const char* string)
{
        dbg_result_t result;
        int          retval;

        assert(string);

        if (!initialized) {
                fprintf(stddbg_out,
                        "Cannot execute command, "
                        "debugger not yet initialized\n");
                return 0;
        }

        /*
         * Copy the input string into our buffer so we can change it at our
         * own need
         */
        if ((strlen(string) + 1) > CONFIG_DEBUGGER_LINE_LENGTH) {
                fprintf(stddbg_out, "Line is too long ...\n");
                return DBG_RESULT_ERROR_LINE_TOOLONG;
        }
        strncpy(dbg_buffer, string, CONFIG_DEBUGGER_LINE_LENGTH);

        result = execute(stddbg_out, dbg_buffer);
        if (result == DBG_RESULT_OK || result == DBG_RESULT_EXIT) {
                retval = 1;
        } else {
                retval = 0;
        }

        return retval;
}

/*
 * NOTE:
 *     This is an entry-point for the debugger
 */
int dbg_enter(void)
{
        dbg_command_t*  temp_cmd;
        dbg_variable_t* temp_var;
        int             complete;

        if (!initialized) {
                fprintf(stddbg_out,
                        "Cannot enter debugger, it's not yet initialized\n");
                return 0;
        }

        interrupts_lock();

        /* First execute the on-enter actions */
        fprintf(stddbg_out, "Entering integrated debugger\n");

        /* Variable specific action */
        temp_var = variables;
        while (temp_var) {
                /* It has a on-enter specific action */
                if (temp_var->actions.on_enter) {
                        /* Let us execute it */
                        if (temp_var->actions.on_enter(stddbg_out) !=
                            DBG_RESULT_OK) {
                                assert(temp_var->name);
                                dprintf("Problems executing on-enter action "
                                        "for variable '%s'\n", temp_var->name);
                        }
                }
                temp_var = temp_var->next;
        }

        /* Command specific action */
        temp_cmd = commands;
        while (temp_cmd) {
                /* It has a on-enter specific action */
                if (temp_cmd->actions.on_enter) {
                        /* Let us execute it */
                        if (temp_cmd->actions.on_enter(stddbg_out) !=
                            DBG_RESULT_OK) {
                                assert(temp_cmd->name);
                                dprintf("Problems executing on-enter action "
                                        "for command '%s'\n", temp_cmd->name);
                        }
                }
                temp_cmd = temp_cmd->next;
        }

        /* Then perform the debugger main-loop */
        dprintf("Debugger main-loop\n");

        complete = 0;
        while (!complete) {
                dbg_result_t result;

                /* Get a new line */
                if (!getline(dbg_buffer, sizeof(dbg_buffer),
                             stddbg_in, stddbg_out)) {
                        dprintf("Cannot get a new line ?\n");
                        break;
                }
                dprintf("Line is = '%s'\n", dbg_buffer);

                /* Do the real work */
                result = execute(stddbg_out, dbg_buffer);
                if (result == DBG_RESULT_EXIT) {
                        /* Command went OK and we need exiting the debugger */
                        complete = 1;
                } else if (result == DBG_RESULT_OK) {
                        /* Command went OK */
                } else {
                        /* Commant KO. We must go on even if it has gone bad */
                }

                /* Then pass to the next command baby ;-) */
        }

        /* Then execute the on-leave actions */
        fprintf(stddbg_out, "Bye bye ...\n");

        /* Variable specific action */
        temp_var = variables;
        while (temp_var) {
                /* It has an on-enter specific action */
                if (temp_var->actions.on_leave) {
                        /* Let us execute it */
                        if (temp_var->actions.on_leave(stddbg_out) !=
                            DBG_RESULT_OK) {
                                assert(temp_var->name);
                                dprintf("Problems executing on-leave action "
                                        "for variable '%s'\n", temp_var->name);
                        }
                }
                temp_var = temp_var->next;
        }

        /* Command specific action */
        temp_cmd = commands;
        while (temp_cmd) {
                /* It has a on-leave specific action */
                if (temp_cmd->actions.on_leave) {
                        /* Let us execute it */
                        if (temp_cmd->actions.on_leave(stddbg_out) !=
                            DBG_RESULT_OK) {
                                assert(temp_cmd->name);
                                dprintf("Problems executing on-leave action "
                                        "for command '%s'\n", temp_cmd->name);
                        }
                }
                temp_cmd = temp_cmd->next;
        }

        interrupts_unlock();

        return 1;
}

void dbg_fini(void)
{
        assert(initialized);

#if CONFIG_DEBUGGER_HISTORY
        dbg_history_fini();
#endif

        dbg_commands_fini();
        dbg_variables_fini();

        initialized = 0;
}
#endif /* CONFIG_DEBUGGER */
