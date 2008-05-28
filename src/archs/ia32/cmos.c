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
#include "libc/stdio.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "arch/cmos.h"
#include "arch/asm.h"
#include "arch/port.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"
#include "libs/bcd.h"

#if CONFIG_CMOS_DEBUG
#define dprintf(F,A...) printf("cmos: " F,##A)
#else
#define dprintf(F,A...)
#endif

/* For infos: http://ivs.cs.uni-magdeburg.de/~zbrog/asm/cmos.html */

#define RTC_INDEX 0x70
#define RTC_IO    0x71

#define RTC_SEC         0x0     /* Seconds register */
#define RTC_SEC_ALRM    0x1     /* Seconds register for alarm */
#define RTC_MIN         0x2     /* Minutes register */
#define RTC_MIN_ALRM    0x3     /* Minutes register for alarm */
#define RTC_HOUR        0x4     /* Hours register */
#define RTC_HOUR_ALRM   0x5     /* Hours register for alarm */
#define RTC_WDAY        0x6     /* Day of the week, 1..7, Sunday = 1 */
#define RTC_MDAY        0x7     /* Day of the month, 1..31 */
#define RTC_MONTH       0x8     /* Month, 1..12 */
#define RTC_YEAR        0x9     /* Year, 0..99 */

#define RTC_REGA        0xA
#define RTC_REGA_UIP    0x80    /* Update in progress. When clear, no update
				 * will occur for 244 micro seconds
				 */

#define RTC_REGB        0xB
#define RTC_REGB_SET    0x80    /* Inhibit updates */
#define RTC_REGB_PIE    0x40    /* Enable periodic interrupts */
#define RTC_REGB_AIE    0x20    /* Enable alarm interrupts */
#define RTC_REGB_UIE    0x10    /* Enable update ended interrupts */
#define RTC_REGB_SQWE   0x08    /* Enable square wave output */
#define RTC_REGB_DM_BCD 0x04    /* Data is in BCD (otherwise binary) */
#define RTC_REGB_24     0x02    /* Count hours in 24-hour mode */
#define RTC_REGB_DSE    0x01    /* Automatic daylight savings updates */

/* Contents of the general purpose CMOS RAM (source IBM reference manual) */
#define CMOS_STATUS             0xE
#define CMOS_STATUS_LOST_POWER  0x80    /* Chip lost power */
#define CMOS_STATUS_BAD_CHKSUM  0x40    /* Checksum is incorrect */
#define CMOS_STATUS_BAD_CONFIG  0x20    /* Bad configuration info */
#define CMOS_STATUS_BAD_MEMSIZE 0x10    /* Wrong memory size */
#define CMOS_STATUS_BAD_HD      0x08    /* Harddisk failed */
#define CMOS_STATUS_BAD_TIME    0x04    /* CMOS time is invalid */

int cmos_read(uint_t pos)
{
	int r;

	port_out8(RTC_INDEX, pos);
	r = port_in8(RTC_IO);

	return r;
}

void cmos_write(uint_t  pos,
		uint8_t value)
{
	port_out8(RTC_INDEX, pos);
	port_out8(RTC_IO, value);
}

static int cmos_time_format_bcd(void)
{
	int bcd;

	bcd = (cmos_read(RTC_REGB) & RTC_REGB_DM_BCD) ? 0 : 1;

	/* dprintf("date/time is in %s format\n", bcd ? "BCD" : "HEX"); */

	return bcd;
}

static int cmos_time_format_h12(void)
{
	int h12;

	h12 = (cmos_read(RTC_REGB) & RTC_REGB_24) ? 0 : 1;

	/* dprintf("date/time is in %s format\n", h12 ? "H12" : "H24"); */

	return h12;
}

static int cmos_state(void)
{
	int state;

	state = cmos_read(CMOS_STATUS);
	if (state & CMOS_STATUS_LOST_POWER) {
		dprintf("CMOS RTC lost power error ...\n");
		return 0;
	}
	if (state & CMOS_STATUS_BAD_CHKSUM) {
		dprintf("CMOS RAM checksum is bad ...\n");
		return 0;
	}
	if (state & CMOS_STATUS_BAD_TIME) {
		dprintf("CMOS time invalid ...\n");
		return 0;
	}

	return 1;
}

int cmos_time_get(cmos_time_t * time)
{
	int       sec,min,hour,day,month,year;
	int       century;
	int       bcd_format;
	int       h12_format;

	assert(time);

	if (!cmos_state()) {
		return 0;
	}

	/*
	 * NOTE:
	 *
	 *   The format of the ten clock data registers (bytes 00h-09h) is:
	 *
	 *   00h Seconds       (BCD 00-59, Hex 00-3B) Note: Bit 7 is read only
	 *   01h Second Alarm  (BCD 00-59, Hex 00-3B; "don't care" if C0-FF)
	 *   02h Minutes       (BCD 00-59, Hex 00-3B)
	 *   03h Minute Alarm  (BCD 00-59, Hex 00-3B; "don't care" if C0-FF))
	 *   04h Hours         (BCD 00-23, Hex 00-17 if 24 hr mode)
	 *                     (BCD 01-12, Hex 01-0C if 12 hr am)
	 *                     (BCD 81-92. Hex 81-8C if 12 hr pm)
	 *   05h Hour Alarm    (same as hours; "don't care" if C0-FF))
	 *   06h Day of Week   (01-07 Sunday=1)
	 *   07h Date of Month (BCD 01-31, Hex 01-1F)
	 *   08h Month         (BCD 01-12, Hex 01-0C)
	 *   09h Year          (BCD 00-99, Hex 00-63)
	 */

	/*  Are the values in BCD or HEX format?  */
	bcd_format = cmos_time_format_bcd();

	/* Time is 12 or 24 h ? */
	h12_format = cmos_time_format_h12();

	/* XXX FIXME: We could hang here if the clock is dead ... */
	while (cmos_read(RTC_REGA) & RTC_REGA_UIP) {
		dprintf("CMOS update in progress ...\n");
	}

	sec   = cmos_read(RTC_SEC);
	min   = cmos_read(RTC_MIN);
	hour  = cmos_read(RTC_HOUR);
	day   = cmos_read(RTC_MDAY);
	month = cmos_read(RTC_MONTH);
	year  = cmos_read(RTC_YEAR);

	/* Convert h12 to h24 */
	if (bcd_format) {
		/* Fix hour */
		if (h12_format) {
			if ((hour >= 1) && (hour <= 12)) {
				/* am */
				hour = hour - 1;
			} else if ((hour >= 81) && (hour <= 92)) {
				/* pm */
				hour = hour - 81;
			} else {
				dprintf("Wrong time !!!\n");
			}
		}
	} else {
		/* Fix hour */
		if (h12_format) {
			if ((hour >= 0x01) && (hour <= 0x0C)) {
				/* am */
				hour = hour - 1;
			} else if ((hour >= 0x81) && (hour <= 0x8C)) {
				/* pm */
				hour = hour - 0x81;
			} else {
				dprintf("Wrong time !!!\n");
			}
		}
	}

	/* Convert time to int */
	if (bcd_format) {
		sec   = BCD2INT(sec);
		min   = BCD2INT(min);
		hour  = BCD2INT(hour);
		day   = BCD2INT(day);
		month = BCD2INT(month);
		year  = BCD2INT(year);
	}

	/*
	 *  TODO:  This fails if century is > 30, but I don't know of
	 *  any other good way to protect against invalid century values.
	 *  Century is always in BCD format (? TODO).
	 *
	 *  If the century value is > 30, we assume that it is invalid.
	 *  To get the correct century value, we need to use the windowing
	 *  trick:   we assume that the century is either 19 or 20.
	 *  NOTE however that this will fail after the year 2069.
	 */
	century = BCD2INT(cmos_read(0x32));
	if (century > 30) {
		dprintf("Invalid century %i\n", century);
		if (year >= 70) {
			century = 19;
		} else {
			century = 20;
		}
	}

	year += (100 * century);

	time->sec   = sec;
	time->min   = min;
	time->hour  = hour;
	time->day   = day;
	time->month = month;
	time->year  = year;

	return 1;
}

int cmos_init(void)
{
	dprintf("Initialized successfully\n");

	return 1;
}

void cmos_fini(void)
{
	dprintf("Finalized successfully\n");
}

#if CONFIG_DEBUGGER
static dbg_result_t command_cmos_on_execute(FILE* stream,
					    int   argc,
					    char* argv[])
{
	cmos_time_t t;

	assert(stream);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "CMOS:\n");

	if (!cmos_time_get(&t)) {
		return DBG_RESULT_ERROR;
	}

	fprintf(stream, "  %d:%d:%d %d/%d/%d (%s/%s)\n",
		t.hour, t.min,   t.sec,
		t.day,  t.month, t.year,
		cmos_time_format_bcd() ? "BCD" : "HEX",
		cmos_time_format_h12() ? "H12" : "H24");

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(cmos,
		    "Show cmos time",
		    NULL,
		    NULL,
		    command_cmos_on_execute,
		    NULL);
#endif
