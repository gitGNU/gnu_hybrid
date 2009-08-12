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

#ifndef CORE_DBG_DEBUGGER_DEBUGGER_H
#define CORE_DBG_DEBUGGER_DEBUGGER_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"

__BEGIN_DECLS

#if CONFIG_DEBUGGER

typedef enum {
	DBG_RESULT_OK,
	DBG_RESULT_EXIT,

	/*
	 * NOTE:
	 *     All other RESULTs must be error related (see dbg_execute()
	 *     and dbg_enter() functions)
	 */
	DBG_RESULT_ERROR,
	DBG_RESULT_ERROR_LINE_TOOLONG,
	DBG_RESULT_ERROR_MISSING_PARAMETERS,
	DBG_RESULT_ERROR_TOOMANY_PARAMETERS,
	DBG_RESULT_ERROR_WRONG_PARAMETERS,
	DBG_RESULT_ERROR_VALUE_TOOHIGH,
	DBG_RESULT_ERROR_VALUE_TOOLOW,
} dbg_result_t;

struct dbg_command {
	const char*          name;
	struct {
		const char*  short_form;     /* Do not use '\n' here ! */
		const char*  long_form;      /* Do not use '\n' here ! */
	} help;

	struct {
		dbg_result_t (* on_enter)(FILE* stream);
		dbg_result_t (* on_execute)(FILE* stream,
					    int   argc,
					    char* argv[]);
		dbg_result_t (* on_leave)(FILE* stream);
	} actions;

	struct dbg_command*  next; /* Do not use ! */
};

typedef struct dbg_command dbg_command_t;

#define DBG_COMMAND_VAR(NAME) __CONCAT(__dbg_cmd_var,NAME)
#define DBG_COMMAND_PTR(NAME) __CONCAT(__dbg_cmd_ptr_,NAME)

/*
 * NOTE:
 *     Do not declare as static or it will be discarded by the linker
 *     and you'll surely head into problems
 */
#define DBG_COMMAND_DECLARE(NAME,SHELP,LHELP,ENTER,EXEC,LEAVE)		\
dbg_command_t DBG_COMMAND_VAR(NAME) UNUSED = {				\
	__STRING(NAME),	{						\
		(SHELP), (LHELP)					\
	}, {								\
		(ENTER),						\
 		(EXEC),							\
		(LEAVE)							\
	},								\
	NULL								\
};									\
									\
dbg_command_t* DBG_COMMAND_PTR(NAME) SECTION(".dbgcmds") UNUSED =	\
	& DBG_COMMAND_VAR(NAME)

typedef enum {
	DBG_VAR_RO = 1,
	DBG_VAR_RW = 2
} dbg_variable_type_t;

struct dbg_variable {
	const char*          name;
	const char*          help;

	struct {
		int          min;
		int          max;
	} range;

	int                  value;
	dbg_variable_type_t  type;

	struct {
		dbg_result_t (* on_enter)(FILE* stream);
		dbg_result_t (* on_set)(FILE* stream, int value);
		dbg_result_t (* on_get)(FILE* stream, int* value);
		dbg_result_t (* on_leave)(FILE* stream);
	} actions;

	struct dbg_variable* next; /* Do not use ! */
};

typedef struct dbg_variable dbg_variable_t;

#define DBG_VARIABLE_VAR(NAME) __CONCAT(__dbg_var_var_,NAME)
#define DBG_VARIABLE_PTR(NAME) __CONCAT(__dbg_var_ptr_,NAME)

/* NOTE: Do not declare as static or you'll head into problems */
#define DBG_VARIABLE_DECLARE(NAME,HELP,MIN,MAX,VAL,TYPE,ENTER,SET,GET,LEAVE) \
dbg_variable_t DBG_VARIABLE_VAR(NAME) UNUSED = {			     \
	__STRING(NAME),							     \
	(HELP), {							     \
		(MIN), (MAX)						     \
	},								     \
	(VAL),								     \
	(TYPE), {							     \
		(ENTER),						     \
		(SET),							     \
		(GET),							     \
		(LEAVE)							     \
	},								     \
	NULL								     \
};									     \
									     \
dbg_variable_t* DBG_VARIABLE_PTR(NAME) SECTION(".dbgvars") UNUSED =	     \
	& DBG_VARIABLE_VAR(NAME)

int                   dbg_init(void);

/* This is the debugger entry point */
int                   dbg_enter(void);
/* Executes the given command */
int                   dbg_execute(const char* string);

int                   dbg_command_add(dbg_command_t* command);
int                   dbg_command_remove(dbg_command_t* command);
const dbg_command_t*  dbg_command_lookup(const char* string);

int                   dbg_commands_config(dbg_command_t** start,
					  dbg_command_t** stop);
int                   dbg_commands_unconfig(dbg_command_t** start,
					    dbg_command_t** stop);

int                   dbg_variable_add(dbg_variable_t* variable);
int                   dbg_variable_remove(dbg_variable_t* variable);
const dbg_variable_t* dbg_variable_lookup(const char* string);
dbg_result_t          dbg_variable_set(const char* string, int value);
dbg_result_t          dbg_variable_get(const char* string, int* value);

int                   dbg_variables_config(dbg_variable_t** start,
					   dbg_variable_t** stop);
int                   dbg_variables_unconfig(dbg_variable_t** start,
					     dbg_variable_t** stop);

int                   dbg_history_init(void);
void                  dbg_history_clear(void);
int                   dbg_history_count(void);
int                   dbg_history_add(const char* string);
const char*           dbg_history_retrieve(int indx);
void                  dbg_history_fini(void);

int                   dbg_completion_init(void);
void                  dbg_completion_fini(void);

void                  dbg_fini(void);

#endif /* CONFIG_DEBUGGER */

__END_DECLS

#endif /* CORE_DBG_DEBUGGER_DEBUGGER_H */
