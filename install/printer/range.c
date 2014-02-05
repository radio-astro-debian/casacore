
#include <ctype.h>
#include <stdio.h>

/*
 * Return 1 if the given number is in the specified range,
 * -1 if there is an error in the range specification,
 * 0 otherwise
 *
 * Ranges have a format *similar* to that used by [nt]roff; i.e.,
 * a comma separated list of specifiers of the form:
 *	1) n	means     x such that x = n
 *	2) :n	means all x such that x <= n
 *	3) n:	means all x such that x >= n
 *	4) n:m	means all x such that n <= x <= m
 *	5) :	means all x
 * n is an integer
 *
 * Problems:
 * The routine prints an error message if the range is strange - this might
 * not always be desirable.
 *
 * I've left the error checking in inrange; it is redundant (I hope!)
 * Nov/85 BJB
 * ========================================================================
 * Copyright (c) 1985-2012 Distributed Systems Software
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Barry Brachman
 * Distributed Systems Software
 * brachman@dss.ca
 * ========================================================================
 */
/* $Id$ */

#define SEP_CHAR	':'
#define BAD_CHAR	'?'

static int getnum ();


inrange(num, range)
int num;
char *range;
{
	char *p;
	int type1, type2, val1, val2;

	if (checkrange(range))
		return(-1);
	type1 = type2 = 0;
	p = range;
	while (1) {
		val1 = getnum(&p, &type1);
		if (type1 == BAD_CHAR) {
			fprintf(stderr, "Bad first number in range\n");
			return(-1);
		}

		switch (*p) {
		case ',':
			if (type1 == SEP_CHAR)	/* just a colon */
				return(1);
			if (num == val1)	/* plain number */
				return(1);
			p++;
			continue;
		case ':':
			p++;
			if (*p == ',' || *p == '\0') {		/* no rhs */
				if (type1 == SEP_CHAR)
					return(1);
				if (num >= val1)
					return(1);
				if (*p == '\0') 
					return(0);
				p++;
				break;
			}

			val2 = getnum(&p, &type2);
			if (type2 == BAD_CHAR) {
				fprintf(stderr, "Bad second number in range\n");
				return(-1);
			}

			if (val1 > val2) {
				fprintf(stderr, "Range values reversed\n");
				return(-1);
			}
			if (type1 == SEP_CHAR && num <= val2)
				return(1);
			if (num >= val1 && num <= val2)
				return(1);
			if (*p == '\0')
				return(0);
			if (*p == ',')
				p++;
			break;
		case '\0':
			if (val1 == num)
				return(1);
			return(0);
		default:
			fprintf(stderr, "Bad character in range\n");
			return(-1);
		}
	}
}

checkrange(range)
char *range;
{
	char *p;
	int type1, type2, val1, val2;

	type1 = type2 = 0;
	p = range;
	while (1) {
		val1 = getnum(&p, &type1);
		if (type1 == BAD_CHAR) {
			fprintf(stderr, "Bad first number in range\n");
			return(-1);
		}

		switch (*p) {
		case ',':
			p++;
			continue;
		case ':':
			p++;
			if (*p == ',' || *p == '\0') {
				if (*p == '\0') 
					return(0);
				p++;
				break;
			}

			val2 = getnum(&p, &type2);
			if (type2 == BAD_CHAR) {
				fprintf(stderr, "Bad second number in range\n");
				return(-1);
			}

			if (val1 > val2) {
				fprintf(stderr, "Range values reversed\n");
				return(-1);
			}
			if (*p == '\0')
				return(0);
			if (*p == ',')
				p++;
			break;
		case '\0':
			return(0);
		default:
			fprintf(stderr, "Bad character in range\n");
			return(-1);
		}
	}
}

static
getnum(pp, type)
char **pp;
int *type;
{
	register int sign, val;
	register char *p;

	p = *pp;
	if (!isdigit(*p) && *p != '-') {
		if (*p == SEP_CHAR)
			*type = SEP_CHAR;
		else
			*type = BAD_CHAR;
		return(0);
	}
	sign = 1;
	if (*p == '-') {
		sign = -1;
		p++;
	}
	if (!isdigit(*p)) {
		*type = BAD_CHAR;
		return(0);
	}
	for (val = 0; isdigit(*p) && *p != '\0'; p++)
		val = val * 10 + *p - '0';
	if (*p != '\0' && *p != ',' && *p != SEP_CHAR) {
		*type = BAD_CHAR;
		return(0);
	}
	*pp = p;
	return(sign * val);
}
