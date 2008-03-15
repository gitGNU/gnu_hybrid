/*
 * Copyright (C) 2008 Francesco Salvestrini
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
#include "libc/stdarg.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "libc/bits/byteswap.h"
#include "core/archs/linker.h"
#include "core/dbg/debug.h"
#include "core/arch/dbg/debugger/gdb.h"
#include "core/dbg/debugger/debugger.h"

#if CONFIG_GDB

#define BANNER           "dbg: "

#if CONFIG_DEBUGGER_DEBUG
#define dprintf(F,A...)  printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

enum {
	GDB_INIT    = 0,
	GDB_CMDREAD,
	GDB_CKSUM1,
	GDB_CKSUM2,
	GDB_WAITACK,
	GDB_QUIT,
	GDB_STATES
};

static char cmd[512];
static char reply[512];
static char safe_mem[512];

static int  cmd_ptr;
static int  checksum;

static int parse_nibble(int input)
{
	int nibble;

	nibble = 0xff;

	if ((input >= '0') && (input <= '9')) {
		nibble = input - '0';
	}
	if ((input >= 'A') && (input <= 'F')) {
		nibble = 0x0a + (input - 'A');
	}
	if ((input >= 'a') && (input <= 'f')) {
		nibble = 0x0a + (input - 'a');
	}

	return nibble;
}

/*
 * GDB protocol ACK & NAK & Reply
 */
static void gdb_ack(void)
{
	putchar('+');
}

static void gdb_nak(void)
{
	putchar('-');
}

static void gdb_resend_reply(void)
{
	puts(reply);
}

static void gdb_reply(const char* fmt, ...)
{
	int i;
	int len;
	int sum;

	va_list args;

	va_start(args, fmt);
	reply[0] = '$';
	vsprintf(reply + 1, fmt, args);
	va_end(args);

	len = strlen(reply);
	sum = 0;
	for (i = 1; i < len; i++) {
		sum += reply[i];
	}
	sum %= 256;

	sprintf(reply + len, "#%02x", sum);

	gdb_resend_reply();
}

#if __BYTE_ORDER == __BIG_ENDIAN
#  define htonl(X) (X)
#else
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define htonl(X) bswap_32(X)
# endif
#endif

static void gdb_regreply(const unsigned int* regs,
			 int                 count)
{
	int i;
	int len;
	int sum;

	reply[0] = '$';
	for (i = 0; i < count; i++) {
		sprintf(reply + 1 + 8 * i, "%08x", htonl(regs[i]));
	}

	len = strlen(reply);
	sum = 0;
	for (i = 1; i < len; i++) {
		sum += reply[i];
	}
	sum %= 256;

	sprintf(reply + len, "#%02x", sum);

	gdb_resend_reply();
}

static void gdb_memreply(const char* bytes, int numbytes)
{
	int i;
	int len;
	int sum;

	reply[0] = '$';
	for (i = 0; i < numbytes; i++) {
		sprintf(reply + 1 + 2 * i, "%02x", (unsigned char) bytes[i]);
	}

	len = strlen(reply);
	sum = 0;
	for (i = 1; i < len; i++) {
		sum += reply[i];
	}
	sum %= 256;

	sprintf(reply + len, "#%02x", sum);

	gdb_resend_reply();
}

/* Checksum verification */
static int gdb_verify_checksum(void)
{
	int i;
	int len;
	int sum;

	len = strlen(cmd);
	sum = 0;
	for (i = 0; i < len; i++) {
		sum += cmd[i];
	}
	sum %= 256;

	return (sum == checksum) ? 1 : 0;
}

/* Command parsing an dispatching */
static int gdb_parse_command(unsigned int* regfile)
{
	if (!gdb_verify_checksum()) {
		gdb_nak();
		return GDB_INIT;
	} else {
		gdb_ack();
	}
	
	switch (cmd[0]) {
	case 'H':
		/*
		 * Command H (actually Hct) is used to select
		 * the current thread (-1 meaning all threads)
		 * We just fake we recognize the the command
		 * and send an 'OK' response.
		 */
		gdb_reply("OK");

		break;

	case 'q':
		/*
		 * There are several q commands:
		 *
		 *     qXXXX        Request info about XXXX.
		 *     QXXXX=yyyy   Set value of XXXX to yyyy.
		 *     qOffsets     Get segment offsets
		 *
		 * Currently we only support the 'qOffsets'
		 * form.
		 *
		 * *Note* that we actually have to lie,
		 * At first thought looks like we should
		 * return '_start', '__data_start' &
		 * '__bss_start', however gdb gets
		 * confused because the kernel link script
		 * pre-links at 0x80000000. To keep gdb
		 * gdb happy we just substract that amount.
		 */
		if (strcmp(cmd + 1, "Offsets")== 0) {
			gdb_reply("Text=%x;Data=%x;Bss=%x",
				  ((unsigned)(&_text)) - 0x80000000,
				  ((unsigned)(&_data)) - 0x80000000,
				  ((unsigned)(&_bss))  - 0x80000000);
		} else {
			gdb_reply("ENS");
		}
		break;

	case '?':
		/*
		 * Command '?' is used for retrieving the signal that stopped
		 * the program. Fully implemeting this command requires help
		 * from the debugger, by now we just fake a SIGKILL ...
		 */
		gdb_reply("S09");	/* SIGKILL = 9 */
		break;

	case 'g':
		/*
		 * Command 'g' is used for reading the register file. Faked by
		 * now.
		 *
		 * For x86 the register order is:
		 *
		 *     eax,ebx,ecx,edx,esp,ebp,esi,edi,eip,eflags,cs,ss,ds,es
		 *
		 * Note that even thought the segment descriptors
		 * are actually 16 bits wide, gdb requires them
		 * as 32 bit integers. Note also that for some
		 * reason (unknown to me) gdb wants the register
		 * dump in *big endian* format.
		 */
		gdb_regreply(regfile, GDB_REGISTER_FILE_COUNT);
		break;
	
	case 'm': {
		char*    ptr;
		unsigned address;
		unsigned len;
		
		/*
		 * The 'm' command has the form mAAA,LLL
		 * where AAA is the address and LLL is the
		 * number of bytes.
		 */
		ptr     = cmd + 1;
		address = 0;
		len     = 0;
		while (ptr && *ptr && (*ptr != ',')) {
			address <<= 4;
			address += parse_nibble(*ptr);
			ptr     += 1;
		}
			
		if (*ptr== ',') {
			ptr += 1;
		}

		while (ptr && *ptr) {
			len <<= 4;
			len += parse_nibble(*ptr);
			ptr += 1;
		}

		if (len > 128) {
			len = 128;
		}

		/*
		 * We cannot directly access the requested memory for gdb may
		 * be trying to access an stray pointer so we copy the memory
		 * to a safe buffer
		 */
		if (!memcpy(safe_mem, (char*)address, len)) {
			gdb_reply("E02");
		} else {
			gdb_memreply(safe_mem, len);
		}

		break;
	}
	
	case 'k': 
		/*
		 * Command 'k' actual semantics is 'kill the damn thing'.
		 * However gdb sends that command when you disconnect
		 * from a debug session. I guess that 'kill' for the
		 * kernel would map to reboot... however that's a
		 * a very mean thing to do, instead we just quit
		 * the gdb state machine and fallback to the regular
		 * kernel debugger command prompt.
		 */
		return GDB_QUIT;
	
	default:
		gdb_reply("E01");
		break;
	}
	
	return GDB_WAITACK;
}

/*
 * GDB protocol state machine
 */
static int gdb_init_handler(int input, unsigned int *regfile)
{
	(void)(regfile);

	switch(input) {
	case '$':
		memset(cmd, 0, sizeof(cmd));
		cmd_ptr = 0;
		return GDB_CMDREAD;
	default:
#if 0
		gdb_nak();
#else
		/*
		 * Looks to me like we should send
		 * a NAK here but it kinda works
		 * better if we just gobble all
		 * junk chars silently
		 */
#endif
		return GDB_INIT;
	}
}

static int gdb_cmdread_handler(int input, unsigned int *regfile)
{
	(void)(regfile);

	switch(input) {
	case '#':
		return GDB_CKSUM1;
	default:
		cmd[cmd_ptr] = input;
		cmd_ptr += 1;
		return GDB_CMDREAD;
	}
}

static int gdb_cksum1_handler(int input, unsigned int *regfile)
{
	int nibble;

	nibble = parse_nibble(input);

	(void)(regfile);

	if (nibble == 0xff) {
#if 0
		gdb_nak();
		return GDB_INIT;
#else
		/*
		 * Looks to me like we should send
		 * a NAK here but it kinda works
		 * better if we just gobble all
		 * junk chars silently
		 */
#endif
	}

	checksum = nibble << 4;

	return GDB_CKSUM2;
}

static int gdb_cksum2_handler(int input, unsigned int *regfile)
{
	int nibble;
	
	nibble = parse_nibble(input);
	if (nibble == 0xff) {
#if 0
		gdb_nak();
		return GDB_INIT;
#else
		/*
		 * Looks to me like we should send
		 * a NAK here but it kinda works
		 * better if we just gobble all
		 * junk chars silently
		 */
#endif
	}

	checksum += nibble;

	return gdb_parse_command(regfile);
}

static int gdb_waitack_handler(int input, unsigned int *regfile)
{
       	(void)(regfile);

	switch(input) {
	case '+':
		return GDB_INIT;
	case '-':
		gdb_resend_reply();
		return GDB_WAITACK;
	default:
		/*
		 * Looks like gdb and us are out of synch,
		 * send a NAK and retry from GDB_INIT state.
		 */
		gdb_nak();
		return GDB_INIT;
	}
}

static int gdb_quit_handler(int input, unsigned int *regfile)
{
	(void)(input);
	(void)(regfile);

	/* Actually we should never be here ... */
	return GDB_QUIT;
}

static int (*dispatch_table[GDB_STATES])(int, unsigned int*) =
{
	&gdb_init_handler,
	&gdb_cmdread_handler,
	&gdb_cksum1_handler,
	&gdb_cksum2_handler,
	&gdb_waitack_handler,
	&gdb_quit_handler
};

static int gdb_state_dispatch(int curr, int input, unsigned int* regfile)
{
	if (curr < GDB_INIT) {
		return GDB_QUIT;
	}
	if (curr >= GDB_STATES) {
		return GDB_QUIT;
	}

	return dispatch_table[curr](input, regfile);
}

int gdb_run(void)
{
	int state;
	int c;

	state = GDB_INIT;
	while (state != GDB_QUIT) {
		c     = getchar();
		state = gdb_state_dispatch(state, c, NULL);
	}

	return 1;
}

static dbg_result_t command_gdb_on_execute(FILE* stream,
					   int   argc,
					   char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Waiting for gdb ...\n");
	if (!gdb_run()) {
		return DBG_RESULT_ERROR;
	}
	
	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(gdb,
		    "Attach to external gdb",
		    NULL,
		    NULL,
		    command_gdb_on_execute,
		    NULL);

#endif /* CONFIG_GDB */
