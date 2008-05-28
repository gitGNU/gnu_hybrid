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
#include "core/time.h"
#include "libc/ctype.h"
#include "libc/unistd.h"
#include "archs/arch.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"

#if CONFIG_MORSE

/*
 * These functions transmit a dot, a dash, a letter space, and a word space.
 *
 * NOTE:
 *     A single unit space is automatically transmitted after each dot or
 *     dash, so the letter_space() function produces only a two-unit pause.
 *     The word_space() function produces only a four-unit pause because the
 *     three-unit letter space has already occurred following the previous
 *     letter.
 */

#define SPACE_MASK (1 << 15)
#define BIT_MASK   0xfe
#define UNIT_TIME  18
#define FREQUENCY  1500

/* Produce a gap between the dots and dashes within a character */
static void intracharacter_gap(void)
{
#if CONFIG_MORSE_AUDIBLE
	delay_ms(UNIT_TIME);
#endif
#if CONFIG_MORSE_VISIBLE
#endif
}

/* Produce a gap between letters */
static void short_gap(void)
{
#if CONFIG_MORSE_AUDIBLE
	delay_ms(UNIT_TIME * 3);
#endif
#if CONFIG_MORSE_VISIBLE
#endif
}

/* Produce a gap between words */
static void medium_gap(void)
{
#if CONFIG_MORSE_AUDIBLE
	delay_ms(UNIT_TIME * 7);
#endif
#if CONFIG_MORSE_VISIBLE
	printf(" ");
#endif
}

/* Produce a gap between sentences */
static void long_gap(void)
{
#if CONFIG_MORSE_AUDIBLE
	delay_ms(UNIT_TIME * 10);
#endif
#if CONFIG_MORSE_VISIBLE
	printf("\n");
#endif
}

/* Send a dot and a space */
static void send_dot(void)
{
#if CONFIG_MORSE_AUDIBLE
	arch_dbg_beep_on(FREQUENCY);
	intracharacter_gap();
	arch_dbg_beep_off();
#endif
#if CONFIG_MORSE_VISIBLE
	printf(".");
#endif
}

/* Send a dash and a space */
static void send_dash(void)
{
#if CONFIG_MORSE_AUDIBLE
	arch_dbg_beep_on(FREQUENCY);
	intracharacter_gap();
	intracharacter_gap();
	intracharacter_gap();
	arch_dbg_beep_off();
#endif
#if CONFIG_MORSE_VISIBLE
	printf("-");
#endif
}

static unsigned int codes[64] = {
	SPACE_MASK,                            /* Entry 0 = space (0x20) */
	 0,   0,   0,   0,   0,   0,   0,   0, /* ! " # $ % & " (        */
	 0,   0,   0, 115,  49, 106,  41,      /* ) * + , - . /          */
	63,  62,  60,  56,  48,  32,  33,  35, /* 0 1 2 3 4 5 6 7        */
	39,  47,   0,   0,   0,   0,   0,  76, /* 8 9 :; < = > ?         */
	 0,   6,  17,  21,   9,   2,  20,  11, /* @ A B C D E F G        */
	16,   4,  30,  13,  18,   7,   5,  15, /* H I J K L M N O        */
	22,  27,  10,   8,   3,  12,  24,  14, /* P Q R S T U V W        */
	25,  29,  19                           /* X Y Z                  */
};

/*
 * This function transmits the string pointed to by the 'message' argument in
 * Morse code using the architecture specific functions. The speed is set by
 * the UNIT_TIME constant.
 *
 * A static table translates from ASCII to Morse code. Each entry is an
 * unsigned integer, where a zero represents a dot and a one represents a dash.
 * No more than 14 bits may be used. Setting bit 15 produces a word space
 * regardless of any other pattern.
 *
 * The Morse code pattern is taken from bit 0, and is shifted right each time
 * an element is sent. A special "marker bit" follows the complete Morse
 * pattern. This marker bit is tested before transmitting each bit; if there
 * are no 1's in bits 1..15, the complete character has been sent.
 *
 * For example, an "L" would be 0000000000010010, with bit zero containing the
 * first dot, bit one the dash, etc. The marker bit is in bit 4.
 */
void morse(const char *message)
{
	unsigned int c;

	assert(message);

	while ((c = *message++) != '\0') {
		/* No lower-case Morse characters */
		c = toupper(c);

		/* Adjust for zero-based table */
		c -= ' ';

		if (/* c < 0 || */ c > 58) {
			/* If out of range, ignore it */
			continue;
		}

		/* Look up Morse pattern from table */
		c = codes[c];

		/* If the space bit is set ... */
		if (c & SPACE_MASK) {
			/* ... send a word space and go on */
			medium_gap();
			continue;
		}

		/* Transmit one character bit by bit */
		while (c & BIT_MASK) {
			if (c & 1) {
				send_dash();
			} else {
				send_dot();
			}
			intracharacter_gap();

			c >>= 1;
		}

		short_gap();
	}

	long_gap();
}

#if CONFIG_DEBUGGER
static dbg_result_t command_morse_on_execute(FILE* stream,
					     int   argc,
					     char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc == 0) {
		return DBG_RESULT_ERROR_MISSING_PARAMETERS;
	} else if (argc == 1) {
		assert(argv[0]);

		morse(argv[0]);
	} else {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(morse,
		    "Morse an input text",
		    NULL,
		    NULL,
		    command_morse_on_execute,
		    NULL);

#endif /* CONFIG_DEBUGGER */
#endif /* CONFIG_MORSE */
