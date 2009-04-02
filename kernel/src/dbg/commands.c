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
#include "libs/debug.h"
#include "dbg/debugger.h"

#if CONFIG_DEBUGGER

#define BANNER           "dbg: "

#if CONFIG_DEBUGGER_DEBUG
#define dprintf(F,A...)  printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

dbg_command_t* commands = NULL;

const dbg_command_t* dbg_command_lookup(const char* string)
{
	dbg_command_t* temp;

	assert(string);

	dprintf("Lookup for command '%s'\n", string);

	temp = commands;
	while (temp) {
		if (!strcmp(temp->name,  string)) {
			dprintf("Found!\n");
			return temp;
		}

		temp = temp->next;
	}

	dprintf("Not found\n");

	return NULL;
}

int dbg_command_add(dbg_command_t* command)
{
	assert(command);

	/* We need the command name */
	assert(command->name);

	/* We need at least an action */
	assert(command->actions.on_execute ||
	       command->actions.on_enter   ||
	       command->actions.on_leave);

	/* We should be safe :-) */
	command->next = commands;
	commands      = command;

	dprintf("Command `%s` added successfully\n", command->name);

	return 1;
}

int dbg_command_remove(dbg_command_t* command)
{
	dbg_command_t* prev;
	dbg_command_t* curr;

	assert(command);
	assert(command->name);

	/* Remove a command from the debugger commands list */

	prev = NULL;
	curr = commands;
	while (curr) {
		if (curr->name == command->name) {
			/* Found it, now simply remove it from the list */
			if (prev) {
				/* Not head element */
				prev->next = curr->next;
			} else {
				/* Head element */
				commands = curr->next;
			}

			/* Command successfully removed */
			return 1;
		}

		/* Not found, next please ... */
		prev = curr;
		curr = curr->next;
	}

	/* Huh ... command not found ... */
	dprintf("Command not found, removal unsuccessful\n");

	return 0;
}

int dbg_commands_config(dbg_command_t** start,
			dbg_command_t** stop)
{
	dbg_command_t** tmp;

	assert(start);
	assert(stop);
	assert(start < stop);

	dprintf("Configuring debugger commands (start = 0x%x, stop = 0x%x)\n",
		start, stop);

	tmp = start;
	while (tmp < stop) {
		assert(tmp);

		dprintf("Command (0x%x)\n", *tmp);

		assert((*tmp)->name);

		dprintf("  Name      = '%s'\n",
			(*tmp)->name);
		dprintf("  Short     = '%s'\n",
			(*tmp)->help.short_form);
		dprintf("  Long      = '%s'\n",
			(*tmp)->help.long_form);
		dprintf("  Callbacks = 0x%x/0x%x/0x%x\n",
			(*tmp)->actions.on_enter,
			(*tmp)->actions.on_execute,
			(*tmp)->actions.on_leave);

		if (!dbg_command_add(*tmp)) {
			dprintf("Cannot add debugger command '%s'",
				(*tmp)->name);
			return 0;
		}

		tmp++;
	}

	return 1;
}

int dbg_commands_unconfig(dbg_command_t** start,
			  dbg_command_t** stop)
{
	dbg_command_t** tmp;

	assert(start);
	assert(stop);
	assert(start < stop);

	tmp = start;
	while (tmp < stop) {
		assert(tmp);
		assert((*tmp)->name);

		if (!dbg_command_remove(*tmp)) {
			dprintf("Cannot remove debugger command '%s'\n",
				(*tmp)->name);
			return 0;
		}

		tmp++;
	}

	return 1;
}

int dbg_commands_init(void)
{
	assert(!commands);

	dprintf("Initializing debugger commands\n");

	if (!dbg_commands_config(&__DBGCMDS_LIST__, &__DBGCMDS_END__)) {
		dprintf("Cannot initialize debugger commands\n");
		return 0;
	}

	return 1;
}

void dbg_commands_fini(void)
{
	assert(commands);

	dprintf("Finalizing debugger commands\n");

	if (!dbg_commands_unconfig(&__DBGCMDS_LIST__, &__DBGCMDS_END__)) {
		dprintf("Cannot finalize debugger commands\n");
	}
}

#endif /* CONFIG_DEBUGGER */
