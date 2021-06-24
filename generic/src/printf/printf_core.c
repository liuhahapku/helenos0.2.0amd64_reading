/*
 * Copyright (C) 2001-2004 Jakub Jermar
 * Copyright (C) 2006 Josef Cejka
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file	print.c
 * @brief	Printing functions.
 */

#include <printf/printf_core.h>
#include <putchar.h>
#include <print.h>
#include <synch/spinlock.h>
#include <arch/arg.h>
#include <arch/asm.h>

#include <arch.h>

SPINLOCK_INITIALIZE(printflock);			/**< printf spinlock */

#define __PRINTF_FLAG_PREFIX		0x00000001	/**< show prefixes 0x or 0*/
#define __PRINTF_FLAG_SIGNED		0x00000002	/**< signed / unsigned number */
#define __PRINTF_FLAG_ZEROPADDED	0x00000004	/**< print leading zeroes */
#define __PRINTF_FLAG_LEFTALIGNED	0x00000010	/**< align to left */
#define __PRINTF_FLAG_SHOWPLUS		0x00000020	/**< always show + sign */
#define __PRINTF_FLAG_SPACESIGN		0x00000040	/**< print space instead of plus */
#define __PRINTF_FLAG_BIGCHARS		0x00000080	/**< show big characters */
#define __PRINTF_FLAG_NEGATIVE		0x00000100	/**< number has - sign */

#define PRINT_NUMBER_BUFFER_SIZE	(64+5)		/**< Buffer big enought for 64 bit number
							 * printed in base 2, sign, prefix and
							 * 0 to terminate string.. (last one is only for better testing 
							 * end of buffer by zero-filling subroutine)*/

/** Enumeration of possible arguments types.
 */
typedef enum {
	PrintfQualifierByte = 0,
	PrintfQualifierShort,
	PrintfQualifierInt,
	PrintfQualifierLong,
	PrintfQualifierLongLong,
	PrintfQualifierNative,
	PrintfQualifierPointer
} qualifier_t;

static char digits_small[] = "0123456789abcdef"; 	/**< Small hexadecimal characters */
static char digits_big[] = "0123456789ABCDEF"; 	/**< Big hexadecimal characters */

/** Checks c for a digit.
 * @param c One character.
 * @return nonzero if c is from interval '0 to '9'.
 */
static inline int isdigit(int c)
{
	return ((c >= '0' )&&( c <= '9'));
}

/** Compute length of given zero terminated string.
 * @param str Pointer to valid string.
 * @return string length without trailing zero.
 */
static __native strlen(const char *str) 
{
	__native counter = 0;

	while (str[counter] != 0) {
		counter++;
	}

	return counter;
}

/** Print count chars from buffer without adding newline
 * @param buf Buffer with size at least count bytes - NULL pointer NOT allowed!
 * @param count 
 * @param ps output method and its data
 * @return number or printed characters
 */
static int printf_putnchars(const char * buf, size_t count, struct printf_spec *ps)
{
	return ps->write((void *)buf, count, ps->data);
}

/** Print string without added newline
 * @param str string to print
 * @param ps write function specification and support data
 * @return number or printed characters
 */
static int printf_putstr(const char * str, struct printf_spec *ps)
{
	size_t count;
	
	if (str == NULL) {
		return printf_putnchars("(NULL)", 6, ps);
	}

	count = strlen(str);

	return ps->write((void *) str, count, ps->data);
}

/** Print one character to output
 * @param c one character
 * @param ps output method
 * @return number or printed characters
 */
static int printf_putchar(int c, struct printf_spec *ps)
{
	unsigned char ch = c;
	
	return ps->write((void *) &ch, 1, ps->data);
}

/** Print one formatted character
 * @param c character to print
 * @param width 
 * @param flags
 * @return number of printed characters, negative value on fail
 */
static int print_char(char c, int width, __u64 flags, struct printf_spec *ps)
{
	int counter = 0;
	
	if (!(flags & __PRINTF_FLAG_LEFTALIGNED)) {
		while (--width > 0) { 	/* one space is consumed by character itself hence predecrement */
			/* FIXME: painful slow */
			if (printf_putchar(' ', ps) > 0)	
				++counter;
		}
	}
	
 	if (printf_putchar(c, ps) > 0)
		counter++;

	while (--width > 0) { /* one space is consumed by character itself hence predecrement */
		if (printf_putchar(' ', ps) > 0)
			++counter;
	}
	
	return ++counter;
}

/** Print one string
 * @param s string
 * @param width 
 * @param precision
 * @param flags
 * @return number of printed characters or negative value on fail
 */
						
static int print_string(char *s, int width, int precision, __u64 flags, struct printf_spec *ps)
{
	int counter = 0;
	size_t size;
	int retval;

	if (s == NULL) {
		return printf_putstr("(NULL)", ps);
	}
	
	size = strlen(s);

	/* print leading spaces */

	if (precision == 0) 
		precision = size;

	width -= precision;
	
	if (!(flags & __PRINTF_FLAG_LEFTALIGNED)) {
		while (width-- > 0) { 	
			if (printf_putchar(' ', ps) == 1)	
				counter++;
		}
	}

	while (precision > size) {
		precision--;
		if (printf_putchar(' ', ps) == 1)	
			++counter;
	}
	
 	if ((retval = printf_putnchars(s, precision, ps)) < 0) {
		return -counter;
	}
	counter += retval;	

	while (width-- > 0) {
		if (printf_putchar(' ', ps) == 1)	
			++counter;
	}
	
	return counter;
}


/** Print number in given base
 *
 * Print significant digits of a number in given
 * base.
 *
 * @param num  Number to print.
 * @param width
 * @param precision
 * @param base Base to print the number in (should
 *             be in range 2 .. 16).
 * @param flags output modifiers
 * @return number of written characters or EOF
 *
 */
static int print_number(__u64 num, int width, int precision, int base , __u64 flags, struct printf_spec *ps)
{
	char *digits = digits_small;
	char d[PRINT_NUMBER_BUFFER_SIZE];	/* this is good enough even for base == 2, prefix and sign */
	char *ptr = &d[PRINT_NUMBER_BUFFER_SIZE - 1];
	int size = 0; /* size of number with all prefixes and signs */
	int number_size; /* size of plain number */
	char sgn;
	int retval;
	int counter = 0;
	
	if (flags & __PRINTF_FLAG_BIGCHARS) 
		digits = digits_big;	
	
	*ptr-- = 0; /* Put zero at end of string */

	if (num == 0) {
		*ptr-- = '0';
		size++;
	} else {
		do {
			*ptr-- = digits[num % base];
			size++;
		} while (num /= base);
	}
	
	number_size = size;

	/* Collect sum of all prefixes/signs/... to calculate padding and leading zeroes */
	if (flags & __PRINTF_FLAG_PREFIX) {
		switch(base) {
			case 2:	/* Binary formating is not standard, but usefull */
				size += 2;
				break;
			case 8:
				size++;
				break;
			case 16:
				size += 2;
				break;
		}
	}

	sgn = 0;
	if (flags & __PRINTF_FLAG_SIGNED) {
		if (flags & __PRINTF_FLAG_NEGATIVE) {
			sgn = '-';
			size++;
		} else if (flags & __PRINTF_FLAG_SHOWPLUS) {
				sgn = '+';
				size++;
			} else if (flags & __PRINTF_FLAG_SPACESIGN) {
					sgn = ' ';
					size++;
				}
	}

	if (flags & __PRINTF_FLAG_LEFTALIGNED) {
		flags &= ~__PRINTF_FLAG_ZEROPADDED;
	}

	/* if number is leftaligned or precision is specified then zeropadding is ignored */
	if (flags & __PRINTF_FLAG_ZEROPADDED) {
		if ((precision == 0) && (width > size)) {
			precision = width - size + number_size;
		}
	}

	/* print leading spaces */
	if (number_size > precision) /* We must print whole number not only a part */
		precision = number_size;

	width -= precision + size - number_size;
	
	if (!(flags & __PRINTF_FLAG_LEFTALIGNED)) {
		while (width-- > 0) { 	
			if (printf_putchar(' ', ps) == 1)	
				counter++;
		}
	}
	
	
	/* print sign */
	if (sgn) {
		if (printf_putchar(sgn, ps) == 1)
			counter++;
	}
	
	/* print prefix */
	
	if (flags & __PRINTF_FLAG_PREFIX) {
		switch(base) {
			case 2:	/* Binary formating is not standard, but usefull */
				if (printf_putchar('0', ps) == 1)
					counter++;
				if (flags & __PRINTF_FLAG_BIGCHARS) {
					if (printf_putchar('B', ps) == 1)
						counter++;
				} else {
					if (printf_putchar('b', ps) == 1)
						counter++;
				}
				break;
			case 8:
				if (printf_putchar('o', ps) == 1)
					counter++;
				break;
			case 16:
				if (printf_putchar('0', ps) == 1)
					counter++;
				if (flags & __PRINTF_FLAG_BIGCHARS) {
					if (printf_putchar('X', ps) == 1)
						counter++;
				} else {
					if (printf_putchar('x', ps) == 1)
						counter++;
				}
				break;
		}
	}

	/* print leading zeroes */
	precision -= number_size;
	while (precision-- > 0) { 	
		if (printf_putchar('0', ps) == 1)
			counter++;
	}

	
	/* print number itself */

	if ((retval = printf_putstr(++ptr, ps)) > 0) {
		counter += retval;
	}
	
	/* print ending spaces */
	
	while (width-- > 0) { 	
		if (printf_putchar(' ', ps) == 1)	
			counter++;
	}

	return counter;
}


/** Print formatted string.
 *
 * Print string formatted according to the fmt parameter
 * and variadic arguments. Each formatting directive
 * must have the following form:
 * 
 * 	\% [ FLAGS ] [ WIDTH ] [ .PRECISION ] [ TYPE ] CONVERSION
 *
 * FLAGS:@n
 * 	- "#" Force to print prefix.
 * 	For conversion \%o the prefix is 0, for %x and \%X prefixes are 0x and 0X
 *	and for conversion \%b the prefix is 0b.
 *
 * 	- "-"	Align to left.
 *
 * 	- "+"	Print positive sign just as negative.
 *
 * 	- " "	If the printed number is positive and "+" flag is not set, print space in
 *	place of sign.
 *
 * 	- "0"	Print 0 as padding instead of spaces. Zeroes are placed between sign and the
 *	rest of the number. This flag is ignored if "-" flag is specified.
 * 
 * WIDTH:@n
 * 	- Specify minimal width of printed argument. If it is bigger, width is ignored.
 * If width is specified with a "*" character instead of number, width is taken from
 * parameter list. And integer parameter is expected before parameter for processed
 * conversion specification. If this value is negative its absolute value is taken
 * and the "-" flag is set.
 *
 * PRECISION:@n
 * 	- Value precision. For numbers it specifies minimum valid numbers.
 * Smaller numbers are printed with leading zeroes. Bigger numbers are not affected.
 * Strings with more than precision characters are cut off.
 * Just as with width, an "*" can be used used instead of a number.
 * An integer value is then expected in parameters. When both width and precision
 * are specified using "*", the first parameter is used for width and the second one
 * for precision.
 * 
 * TYPE:@n
 * 	- "hh"	Signed or unsigned char.@n
 * 	- "h"	Signed or usigned short.@n
 * 	- ""	Signed or usigned int (default value).@n
 * 	- "l"	Signed or usigned long int.@n
 * 	- "ll"	Signed or usigned long long int.@n
 * 	- "z"	__native (non-standard extension).@n
 * 
 * 
 * CONVERSION:@n
 * 	- %	Print percentile character itself.
 *
 * 	- c	Print single character.
 *
 * 	- s	Print zero terminated string. If a NULL value is passed as value, "(NULL)" is printed instead.
 * 
 * 	- P, p	Print value of a pointer. Void * value is expected and it is printed in hexadecimal notation with prefix
 * 	(as with \%#X or \%#x for 32bit or \%#X / \%#x for 64bit long pointers).
 *
 * 	- b	Print value as unsigned binary number. Prefix is not printed by default. (Nonstandard extension.)
 * 
 * 	- o	Print value as unsigned octal number. Prefix is not printed by default. 
 *
 * 	- d,i	Print signed decimal number. There is no difference between d and i conversion.
 *
 * 	- u	Print unsigned decimal number.
 *
 * 	- X, x	Print hexadecimal number with upper- or lower-case. Prefix is not printed by default.
 * 
 * All other characters from fmt except the formatting directives
 * are printed in verbatim.
 *
 * @param fmt Formatting NULL terminated string.
 * @return Number of printed characters or negative value on failure.
 */
int printf_core(const char *fmt, struct printf_spec *ps, va_list ap)
{
	int irqpri;
	int i = 0, j = 0; /**< i is index of currently processed char from fmt, j is index to the first not printed nonformating character */
	int end;
	int counter; /**< counter of printed characters */
	int retval; /**< used to store return values from called functions */
	char c;
	qualifier_t qualifier;	/* type of argument */
	int base;	/**< base in which will be parameter (numbers only) printed */
	__u64 number; /**< argument value */
	size_t	size; /**< byte size of integer parameter */
	int width, precision;
	__u64 flags;
	
	counter = 0;
	
	irqpri = interrupts_disable();
	spinlock_lock(&printflock);

	while ((c = fmt[i])) {
		/* control character */
		if (c == '%' ) { 
			/* print common characters if any processed */	
			if (i > j) {
				if ((retval = printf_putnchars(&fmt[j], (size_t)(i - j), ps)) < 0) { /* error */
					counter = -counter;
					goto out;
				}
				counter += retval;
			}
		
			j = i;
			/* parse modifiers */
			flags = 0;
			end = 0;
			
			do {
				++i;
				switch (c = fmt[i]) {
					case '#': flags |= __PRINTF_FLAG_PREFIX; break;
					case '-': flags |= __PRINTF_FLAG_LEFTALIGNED; break;
					case '+': flags |= __PRINTF_FLAG_SHOWPLUS; break;
					case ' ': flags |= __PRINTF_FLAG_SPACESIGN; break;
					case '0': flags |= __PRINTF_FLAG_ZEROPADDED; break;
					default: end = 1;
				};	
				
			} while (end == 0);	
			
			/* width & '*' operator */
			width = 0;
			if (isdigit(fmt[i])) {
				while (isdigit(fmt[i])) {
					width *= 10;
					width += fmt[i++] - '0';
				}
			} else if (fmt[i] == '*') {
				/* get width value from argument list*/
				i++;
				width = (int)va_arg(ap, int);
				if (width < 0) {
					/* negative width means to set '-' flag */
					width *= -1;
					flags |= __PRINTF_FLAG_LEFTALIGNED;
				}
			}
			
			/* precision and '*' operator */	
			precision = 0;
			if (fmt[i] == '.') {
				++i;
				if (isdigit(fmt[i])) {
					while (isdigit(fmt[i])) {
						precision *= 10;
						precision += fmt[i++] - '0';
					}
				} else if (fmt[i] == '*') {
					/* get precision value from argument list*/
					i++;
					precision = (int)va_arg(ap, int);
					if (precision < 0) {
						/* negative precision means to ignore it */
						precision = 0;
					}
				}
			}

			switch (fmt[i++]) {
				/** TODO: unimplemented qualifiers:
				 * t ptrdiff_t - ISO C 99
				 */
				case 'h':	/* char or short */
					qualifier = PrintfQualifierShort;
					if (fmt[i] == 'h') {
						i++;
						qualifier = PrintfQualifierByte;
					}
					break;
				case 'l':	/* long or long long*/
					qualifier = PrintfQualifierLong;
					if (fmt[i] == 'l') {
						i++;
						qualifier = PrintfQualifierLongLong;
					}
					break;
				case 'z':	/* __native */
					qualifier = PrintfQualifierNative;
					break;
				default:
					qualifier = PrintfQualifierInt; /* default type */
					--i;
			}	
			
			base = 10;

			switch (c = fmt[i]) {

				/*
				* String and character conversions.
				*/
				case 's':
					if ((retval = print_string(va_arg(ap, char*), width, precision, flags, ps)) < 0) {
						counter = -counter;
						goto out;
					};
					
					counter += retval;
					j = i + 1; 
					goto next_char;
				case 'c':
					c = va_arg(ap, unsigned int);
					if ((retval = print_char(c, width, flags, ps)) < 0) {
						counter = -counter;
						goto out;
					};
					
					counter += retval;
					j = i + 1;
					goto next_char;

				/* 
				 * Integer values
				*/
				case 'P': /* pointer */
				       	flags |= __PRINTF_FLAG_BIGCHARS;
				case 'p':
					flags |= __PRINTF_FLAG_PREFIX;
					base = 16;
					qualifier = PrintfQualifierPointer;
					break;	
				case 'b': 
					base = 2;
					break;
				case 'o':
					base = 8;
					break;
				case 'd':
				case 'i':
					flags |= __PRINTF_FLAG_SIGNED;  
				case 'u':
					break;
				case 'X':
					flags |= __PRINTF_FLAG_BIGCHARS;
				case 'x':
					base = 16;
					break;
				/* percentile itself */
				case '%': 
					j = i;
					goto next_char;
				/*
				* Bad formatting.
				*/
				default:
					/* Unknown format
					 *  now, the j is index of '%' so we will
					 * print whole bad format sequence
					 */
					goto next_char;		
			}
		
		
		/* Print integers */
			/* print number */
			switch (qualifier) {
				case PrintfQualifierByte:
					size = sizeof(unsigned char);
					number = (__u64)va_arg(ap, unsigned int);
					break;
				case PrintfQualifierShort:
					size = sizeof(unsigned short);
					number = (__u64)va_arg(ap, unsigned int);
					break;
				case PrintfQualifierInt:
					size = sizeof(unsigned int);
					number = (__u64)va_arg(ap, unsigned int);
					break;
				case PrintfQualifierLong:
					size = sizeof(unsigned long);
					number = (__u64)va_arg(ap, unsigned long);
					break;
				case PrintfQualifierLongLong:
					size = sizeof(unsigned long long);
					number = (__u64)va_arg(ap, unsigned long long);
					break;
				case PrintfQualifierPointer:
					size = sizeof(void *);
					number = (__u64)(unsigned long)va_arg(ap, void *);
					break;
				case PrintfQualifierNative:
					size = sizeof(__native);
					number = (__u64)va_arg(ap, __native);
					break;
				default: /* Unknown qualifier */
					counter = -counter;
					goto out;
			}
			
			if (flags & __PRINTF_FLAG_SIGNED) {
				if (number & (0x1 << (size*8 - 1))) {
					flags |= __PRINTF_FLAG_NEGATIVE;
				
					if (size == sizeof(__u64)) {
						number = -((__s64)number);
					} else {
						number = ~number;
						number &= (~((0xFFFFFFFFFFFFFFFFll) <<  (size * 8)));
						number++;
					}
				}
			}

			if ((retval = print_number(number, width, precision, base, flags, ps)) < 0) {
				counter = -counter;
				goto out;
			};

			counter += retval;
			j = i + 1;
		}	
next_char:
			
		++i;
	}
	
	if (i > j) {
		if ((retval = printf_putnchars(&fmt[j], (__native)(i - j), ps)) < 0) { /* error */
			counter = -counter;
			goto out;
			
		}
		counter += retval;
	}

out:
	spinlock_unlock(&printflock);
	interrupts_restore(irqpri);
	
	return counter;
}

