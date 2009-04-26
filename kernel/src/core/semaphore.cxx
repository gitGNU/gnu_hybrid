//
// Copyright (C) 2008, 2009 Francesco Salvestrini
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
#include "archs/arch.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "core/semaphore.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

// NOTE:
//     We MUST NOT store the sempahore ownership. A semaphore has
//     no ownership (in contrast to mutexes).

int semaphore_init(semaphore_t* semaphore,
		   int          count)
{
	assert(semaphore);

	semaphore->count = count;

	return 1;
}

void semaphore_fini(semaphore_t* semaphore)
{
	assert(semaphore);
}

semaphore_t* semaphore_new(int count)
{
	semaphore_t* tmp;

	tmp = (semaphore_t *) malloc(sizeof(semaphore_t));
	if (!tmp) {
		return NULL;
	}

	if (!semaphore_init(tmp, count)) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

void semaphore_delete(semaphore_t* semaphore)
{
	assert(semaphore);

	semaphore_fini(semaphore);
	free(semaphore);
}

void semaphore_acquire(semaphore_t* semaphore)
{
	assert(semaphore);

	missing();
}

void semaphore_release(semaphore_t* semaphore)
{
	assert(semaphore);

	missing();
}

#if CONFIG_DEBUGGER
static dbg_result_t command_semaphores_on_execute(FILE* stream,
						  int   argc,
						  char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return	DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Semaphores:\n");

	missing();

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(semaphores,
		    "Show semaphores",
		    NULL,
		    NULL,
		    command_semaphores_on_execute,
		    NULL);
#endif

// NOTE:
//     We MUST store the sempahore ownership. A mutex has
//     ownership (in contrast to semaphores).

// NOTE:
//     Should we allow recursive locking ??? This could
//     enhance the recursive shared resource access case ...

int mutex_init(mutex_t * mutex)
{
	assert(mutex);

	if (!semaphore_init(&mutex->semaphore, 1)) {
		return 0;
	}

	return 1;
}

int mutex_fini(mutex_t * mutex)
{
	assert(mutex);

	semaphore_fini(&mutex->semaphore);

	return 1;
}

mutex_t * mutex_new(void)
{
	mutex_t * tmp;

	tmp = (mutex_t *) malloc(sizeof(mutex_t));
	if (!tmp) {
		return NULL;
	}

	if (!mutex_init(tmp)) {
		free(tmp);
		return NULL;
	}

	return tmp;
}

void mutex_delete(mutex_t * mutex)
{
	assert(mutex);

	mutex_fini(mutex);
	free(mutex);
}

void mutex_lock(mutex_t * mutex)
{
	assert(mutex);

	semaphore_acquire(&mutex->semaphore);
}

void mutex_unlock(mutex_t * mutex)
{
	assert(mutex);

	semaphore_release(&mutex->semaphore);
}

int mutex_locked(mutex_t * mutex)
{
	assert(mutex);

	missing();

	return 1;
}

int mutex_trylock(mutex_t * mutex)
{
	assert(mutex);

	missing();

	return 1;
}

#if CONFIG_DEBUGGER
static dbg_result_t command_mutexes_on_execute(FILE* stream,
					       int   argc,
					       char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return	DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Mutexes:\n");

	missing();

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(mutexes,
		    "Show mutexes",
		    NULL,
		    NULL,
		    command_mutexes_on_execute,
		    NULL);
#endif
