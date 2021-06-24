/*
 * Copyright (C) 2005 Martin Decky
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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <panic.h>
#include <arch/debug.h>

#define CALLER       ((__address)__builtin_return_address(0))

#ifndef HERE
/** Current Instruction Pointer address */
#  define HERE ((__address *)0)
#endif

/** Debugging ASSERT macro
 *
 * If CONFIG_DEBUG is set, the ASSERT() macro
 * evaluates expr and if it is false raises
 * kernel panic.
 *
 * @param expr Expression which is expected to be true.
 *
 */
#ifdef CONFIG_DEBUG
#	define ASSERT(expr) if (!(expr)) { panic("assertion failed (%s), caller=%.*p\n", #expr, sizeof(__address) * 2, CALLER); }
#else
#	define ASSERT(expr)
#endif

#define STRING(arg) STRING_ARG(arg)
#define STRING_ARG(arg) #arg

#endif
