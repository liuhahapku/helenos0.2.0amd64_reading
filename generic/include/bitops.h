/*
 * Copyright (C) 2006 Ondrej Palkovsky
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

#ifndef _BITOPS_H_
#define _BITOPS_H_

#include <typedefs.h>


/** Return position of first non-zero bit from left (i.e. [log_2(arg)]).
 *
 * If number is zero, it returns 0
 */
static inline int fnzb32(__u32 arg)
{
	int n = 0;

	if (arg >> 16) { arg >>= 16;n += 16;}
	if (arg >> 8) { arg >>= 8; n += 8;}
	if (arg >> 4) { arg >>= 4; n += 4;}
	if (arg >> 2) { arg >>= 2; n+=2;}
	if (arg >> 1) { arg >>= 1; n+=1;}
	return n;
}

static inline int fnzb64(__u64 arg)
{
	int n = 0;

	if (arg >> 32) { arg >>= 32;n += 32;}
	return n + fnzb32((__u32) arg);
}

#define fnzb(x) fnzb32(x)

#endif
