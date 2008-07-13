//
// Copyright (C) 2008 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "config/config.h"
#include "libc/string.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/ctype.h"
#include "libc/param.h"
#include "libs/debug.h"
#include "core/log.h"
#include "dbg/debugger.h"

__BEGIN_DECLS

#define BANNER          "log: "

#if CONFIG_LOG_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static int   initialized = 0;

static FILE   _stdlog     = FILE_INITIALIZER;
static FILE * stdlog      = &_stdlog;

static int    position    = 0;
static int    length      = 0;
static char   buffer[CONFIG_LOG_SIZE];

/* Utility macro */
#define CHECK_POSITION(P)			\
	__BEGIN_MACRO				\
	assert((P) >= 0);			\
	assert((P) < CONFIG_LOG_SIZE);		\
	__END_MACRO

/* Circular buffer putchar() */
static int log_putchar(int character)
{
	CHECK_POSITION(position);

	buffer[position] = (char) character;
	position         = (position + 1) % CONFIG_LOG_SIZE;

	length           = length + 1;

	length           = MIN(length, CONFIG_LOG_SIZE);

	CHECK_POSITION(position);

	/* Tee the log to stdout */
	putchar(character);

	return (char) character;
}

/* Circular buffer getchar() */
static int log_getchar(void)
{
	char character;

	CHECK_POSITION(position);

	character = buffer[position];
	position  = (position + 1) % CONFIG_LOG_SIZE;

	CHECK_POSITION(position);

	return (char) character;
}

/* XXX FIXME: See the if branch comments */
static int log_fseek(long offset, int whence)
{
	long delta;
	long new_position;

	CHECK_POSITION(position);

	/* Adjust new origin with whence */
	new_position = position;
	switch (whence) {
		case SEEK_SET: new_position = position - length; break;
		case SEEK_CUR:                                   break;
		case SEEK_END:                                   break;
		default:       bug();			         break;
	}
	/* Adjust new origin with offset */
	new_position = new_position + offset;

	delta = new_position - position;
	if (delta < 0 || delta >= CONFIG_LOG_SIZE) {
		/* Out of bounds */
		return EOF;
	}

	position = new_position % CONFIG_LOG_SIZE;

	CHECK_POSITION(position);

	return 0;
}

static long log_ftell(void)
{
	return (long) position;
}

void log_dump(FILE* stream)
{
	int  i;
	long old_position;

	assert(stream);

	CHECK_POSITION(position);

	/* Save the current position */
	old_position = position;

	/* Rewind the stdlog stream */
	position = (position - length) % CONFIG_LOG_SIZE;
	CHECK_POSITION(position);

	/* Dump all the log buffer to the passed stream */
	for (i = 0; (i < length) && (i < CONFIG_LOG_SIZE) ; i++) {
		char c;

		c = buffer[(position + i) % CONFIG_LOG_SIZE];

		/* We shouldn't get non printable chars ... */
		assert(isprint(c) || isspace(c));

		if (fputc(c, stream) == EOF) {
			/* Problems ... let the caller handle them */
			break;
		}
	}

	/* Restore the saved position */
	position = old_position;
	CHECK_POSITION(position);
}

int log_init(void)
{
	assert(!initialized);

	/* Clear the buffer */
	memset(buffer, 0, CONFIG_LOG_SIZE);

	/* Set the position and length */
	position = 0;
	length   = 0;

	/* Set the stream functions */
	FILE_update(stdlog, log_putchar, log_getchar, log_fseek, log_ftell);

	/* Ok */
	initialized = 1;

	dprintf("Logger initialized, buffer size is %d bytes\n",
		CONFIG_LOG_SIZE);

	return 1;
}

int log(log_level_t level, const char* format, ...)
{
	va_list args;
	int     i;

	unused_argument(level);

	dprintf("Logging at level %d\n", level);

	/* XXX FIXME: We need to add the log levels ... */
	va_start(args, format);
	i = vfprintf(stdlog, format, args);
	va_end(args);

	return i;
}

void log_fini(void)
{
	assert(initialized);

	/* Remove the stream hooks */
	FILE_set(stdlog, NULL, NULL, NULL, NULL);

	initialized = 0;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_log_on_execute(FILE* stream,
					   int   argc,
					   char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	log_dump(stream);

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(log,
		    "Dumps the kernel log",
		    NULL,
		    NULL,
		    command_log_on_execute,
		    NULL);
#endif

__END_DECLS
