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
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/stddef.h"
#include "libc/ctype.h"
#include "archs/x86/port.h"
#include "archs/x86/keyboard.h"
#include "core/resource.h"
#include "libs/debug.h"

#if CONFIG_HOSTED_DEBUGGER

#if CONFIG_ATKBD_SCANCODES_DEBUG
#define dprintf(F,A...) printf("host: " F,##A)
#else
#define dprintf(F,A...)
#endif

/* Input buffer / Output buffer */
#define ATKBD_DATA      0x060
/* OUT: Commands / IN: Status */
#define ATKBD_STATUS    0x064
#define ATKBD_COMMAND   0x064

/* XXX-FIXME: Split this macro (maybe building siblings) */
RESOURCE_DECLARE(at_keyboard_data,"AT Keyboard data",			\
		 RSRC_IO,ATKBD_DATA,ATKBD_DATA);
RESOURCE_DECLARE(at_keyboard_stat_data,"AT Keyboard status/command",	\
		 RSRC_IO,ATKBD_DATA,ATKBD_DATA);

#define ATKBD_LALT      0x0100
#define ATKBD_LSHIFT    0x0200
#define ATKBD_LCTRL     0x0400

#define ATKBD_RALT      0x0800
#define ATKBD_RSHIFT    0x0a00
#define ATKBD_RCTRL     0x0f00

#define ATKBD_SCANCODES 128

typedef struct {
	uint8_t normal[ATKBD_SCANCODES];
	uint8_t e1[ATKBD_SCANCODES];
} kbd_states_t;

typedef struct {
	uint16_t normal[ATKBD_SCANCODES];
	uint16_t e1[ATKBD_SCANCODES];
} kbd_maps_t;

static kbd_states_t kbd_states;
static kbd_maps_t   kbd_maps = {
	{ /* NORMAL MAP */

		/* 0x00 */
		0,             KEY_ESCAPE,     KEY_1,          KEY_2,
		KEY_3,         KEY_4,          KEY_5,          KEY_6,
		KEY_7,         KEY_8,          KEY_9,          KEY_0,
		KEY_MINUS,     KEY_EQUALS,     KEY_BACKSPACE,  KEY_TAB,

		/* 0x10 */
		KEY_q,         KEY_w,        KEY_e,           KEY_r,
		KEY_t,         KEY_y,        KEY_u,           KEY_i,
		KEY_o,         KEY_p,        KEY_LEFTBRACKET, KEY_RIGHTBRACKET,
		KEY_LINEFEED,  KEY_LCTRL,    KEY_a,           KEY_s,

		/* 0x20 */
		KEY_d,         KEY_f,          KEY_g,          KEY_h,
		KEY_j,         KEY_k,          KEY_l,          KEY_SEMICOLON,
		KEY_QUOTE,     KEY_BACKQUOTE,  KEY_LSHIFT,     KEY_BACKSLASH,
		KEY_z,         KEY_x,          KEY_c,          KEY_v,

		/* 0x30 */
		KEY_b,         KEY_n,          KEY_m,          KEY_COMMA,
		KEY_PERIOD,    KEY_SLASH,      KEY_RSHIFT,     KEY_ASTERISK,
		KEY_LALT,      KEY_SPACE,      KEY_CAPSLOCK,   KEY_F1,
		KEY_F2,        KEY_F3,         KEY_F4,         KEY_F5,

		/* 0x40 */
		KEY_F6,        KEY_F7,         KEY_F8,         KEY_F9,
		KEY_F10,       KEY_NUMLOCK,    KEY_SCROLLOCK,  KEY_PAD_7,
		KEY_PAD_8,     KEY_PAD_9,      KEY_PAD_MINUS,  KEY_PAD_4,
		KEY_PAD_5,     KEY_PAD_6,      KEY_PAD_PLUS,   KEY_PAD_1,

		/* 0x50 */
		KEY_PAD_2,     KEY_PAD_3,      KEY_PAD_0,      KEY_PAD_PERIOD,
		0,             0,              0,              KEY_F11,
		KEY_F12,       0,              0,              0,
		0,             0,              0,              0,

	}, { /* E1 MAP */

		/* 0x00 */
		0,             0,              0,              0,
		0,             0,              0,              0,
		0,             0,              0,              0,
		0,             0,              0,              0,

		/* 0x10 */
		0,             0,              0,              0,
		0,             0,              0,              0,
		0,             0,              0,              0,
		KEY_PAD_ENTER, KEY_RCTRL,      0,              0,

		/* 0x20 */
		0,             0,              0,              0,
		0,             0,              0,              0,
		0,             0,              0,              0,
		0,             0,              0,              0,

		/* 0x30 */
		0,             0,              0,              0,
		0,             KEY_PAD_DIVIDE, 0,              KEY_PRINT,
		KEY_RALT,      0,              0,              0,
		0,             0,              0,              0,

		/* 0x40 */
		0,             0,              0,              0,
		0,             0,              0,              KEY_HOME,
		KEY_UP,        KEY_PAGEUP,     0,              KEY_LEFT,
		0,             KEY_RIGHT,      0,              KEY_END,

		/* 0x50 */
		KEY_DOWN,      KEY_PAGEDOWN,   KEY_INSERT,     0,
		0,             0,              0,              0,
		0,             0,              0,              KEY_LSUPER,
		KEY_RSUPER,    KEY_MENU,       0,              0
	}
};

static uint8_t*  kbd_state = NULL;
static uint16_t* kbd_map   = NULL;

static int initialized = 0;

static void wait_output(void)
{
	uint8_t status;

	for (;;) {
		status = port_in8(ATKBD_STATUS);
		if (status & 0xC0 /* Bit 7 = PARE, Bit 6 = TIM */) {
			/* Parity error or timeout */
			printf("Keyboard parity error/timeout\n");
			continue;
		}

		if (status & 0x20 /* Bit 5 = AUXB */) {
			/* Data is from mouse (PS/2) */
			printf("Data from mouse ?\n");
			continue;
		}

		/* Other flags: not needed */

		if (status & 0x01 /* Bit 0 = OUTB*/) {
			/* Data is ready */
			break;;
		}
	}
}

int kbd_init(void)
{
	/* Clear the keyboard key states on startup */
	memset(&kbd_states, 0, sizeof(kbd_states));

	initialized = 1;

	/* Enable keyboard */
	port_out8(ATKBD_DATA, 0xF4);

	/* Set keyboard scancode set 2 */
	wait_output();
	port_out8(ATKBD_DATA, 0xF0);

	wait_output();
	port_out8(ATKBD_DATA, 0x02);

	return 1;
}

void kbd_fini(void)
{
	if (initialized) {
		/* Disable keyboard */
		wait_output();
		port_out8(ATKBD_DATA, 0xF5);

		initialized = 0;
	}
}

int kbd_getchar(void)
{
	uint8_t                scancode;
	uint8_t                c;
	static int             seen_E1;
	static key_modifier_t  modifier;

	c       = KEY_UNKNOWN;
	seen_E1 = KMOD_NONE;

	if (!initialized) {
		return EOF;
	}

	for (;;) {
		int key_is_down;
		int key_was_up;

		/* Wait till input is ready */
		wait_output();

		/* Then read a scancode */
		scancode = port_in8(ATKBD_DATA);
		if (scancode == 0x00) {
			/* Keyboard error */
		} else if (scancode == 0xAA) {
			/* Basic Assurance Test OK */
		} else if (scancode == 0xF1) {
			/* Reply to A4:'Password not installed' for some
			 *  devices
			 */
		} else if (scancode == 0xFA) {
			/* Acknowledge */
		} else if (scancode == 0xFC) {
			/* Basic Assurance Test KO */
		} else if (scancode == 0xFD) {
			/* Internal failure */
		} else if (scancode == 0xFE) {
			/* Keyboard fails to ACK, please resend */
		} else if (scancode == 0xFF) {
			/* Keyboard error */
		} else {
			/* Ok! */
		}

		/* If it is a mode escape then register it */

		if (scancode == 0xE0) {
			dprintf("I see E0\n");
		}

		if (scancode == 0xE1) {
			seen_E1 = 1;
			dprintf("I see E1\n");
			continue;
		}

		/* Set the state & map */
		if (seen_E1) {
			kbd_state = kbd_states.e1;
			kbd_map   = kbd_maps.e1;
		} else {
			kbd_state = kbd_states.normal;
			kbd_map   = kbd_maps.normal;
		}

		/* Clear mode (if enabled) */
		if (seen_E1) {
			seen_E1 = 0;
		}

		/* Preserve UP/DOWN indication and remove from the scancode */
		if (scancode & 0x80) {
			/* This is a 'break' scancode: key is UP */
			key_is_down  = 0;
			scancode    &= 0x7F;
			dprintf("Key is down\n");
		} else {
			/* This is a 'make' scancode: key is DOWN */
			key_is_down = 1;
			dprintf("Key is up\n");
		}

		/* Update key UP/DOWN state */
		key_was_up = 0;
		if (key_is_down) {
			/* Key is now DOWN */
			if (kbd_state[scancode] == 0) {
				/* But it was UP ... We got it ! */
				kbd_state[scancode] = 1;
				key_was_up          = 1;
			} else {
				/* It was DOWN, nothing to do ... */
			}
		} else {
			/* Key is now UP */
			if (kbd_state[scancode] == 1) {
				/* it was DOWN */
				kbd_state[scancode] = 0;
			} else {
				/* It was UP, nothing to do ... */
			}
		}
		/* From this point on key_was_up and key_is_down are OK */

		/* Handle modifiers now ! */

		/*
		 * NOTE:
		 *     Verify that we are not trying to access out-of-bounds
		 *     scancodes ...
		 */
		assert(/* (scancode >= 0) && */ (scancode < ATKBD_SCANCODES));

		if (key_was_up && key_is_down) { /* Shitty */
			switch (kbd_map[scancode]) {
			case KEY_RALT:	 modifier |= KMOD_RALT;	   continue;
			case KEY_LALT:	 modifier |= KMOD_LALT;	   continue;
			case KEY_RSHIFT: modifier |= KMOD_RSHIFT;  continue;
			case KEY_LSHIFT: modifier |= KMOD_LSHIFT;  continue;
			case KEY_RCTRL:  modifier |= KMOD_RCTRL;   continue;
			case KEY_LCTRL:  modifier |= KMOD_LCTRL;   continue;
			default:         /* Nothing to do */       break;
			}
		}
		if (!key_was_up && !key_is_down) {  /* Shitty */
			switch (kbd_map[scancode]) {
			case KEY_RALT:	 modifier &= !KMOD_RALT;   continue;
			case KEY_LALT:	 modifier &= !KMOD_LALT;   continue;
			case KEY_RSHIFT: modifier &= !KMOD_RSHIFT; continue;
			case KEY_LSHIFT: modifier &= !KMOD_LSHIFT; continue;
			case KEY_RCTRL:  modifier &= !KMOD_RCTRL;  continue;
			case KEY_LCTRL:  modifier &= !KMOD_LCTRL;  continue;
			default:         /* Nothing to do */       break;
			}
		}

		if (key_was_up && key_is_down) {
			/* We got the key so we need to break the loop */
			break;
		}
	}
	dprintf("Scan 0x%02x, Lookup 0x%02x\n", scancode, c);

	/*
	 * NOTE:
	 *     We must verify that we are not trying to access out-of-bounds
	 *     scancodes ...
	 */
	assert(/* (scancode >= 0) && */ (scancode < ATKBD_SCANCODES));

	c = key2ascii(kbd_map[scancode]);
	if (c == KEY_UNKNOWN) {
		printf("Unknown scancode 0x%02x\n", scancode);
	} else {
		/* Update the key value according to the modifiers */
		if (modifier & KMOD_SHIFT) {
			c = toupper(c);
		}
	}

	return c;
}
#endif /* CONFIG_HOSTED_DEBUGGER */
