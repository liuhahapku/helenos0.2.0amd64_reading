/*
 * Copyright (C) 2001-2004 Jakub Jermar
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

#ifndef __CONDVAR_H__
#define __CONDVAR_H__

#include <arch/types.h>
#include <synch/waitq.h>
#include <typedefs.h>
#include <synch/synch.h>

struct condvar {
	waitq_t wq;
};

#define condvar_wait(cv,mtx) \
	_condvar_wait_timeout((cv),(mtx),SYNCH_NO_TIMEOUT,SYNCH_FLAGS_NONE)
#define condvar_wait_timeout(cv,mtx,usec) \
	_condvar_wait_timeout((cv),(mtx),(usec),SYNCH_FLAGS_NONE)

extern void condvar_initialize(condvar_t *cv);
extern void condvar_signal(condvar_t *cv);
extern void condvar_broadcast(condvar_t *cv);
extern int _condvar_wait_timeout(condvar_t *cv, mutex_t *mtx, __u32 usec, int flags);

#endif
