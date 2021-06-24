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

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

typedef enum {
	SYS_IO = 0,
	SYS_TLS_SET = 1, /* Hardcoded in AMD64, IA32 uspace - psthread.S */
	SYS_THREAD_CREATE,
	SYS_THREAD_EXIT,
	SYS_TASK_GET_ID,
	SYS_FUTEX_SLEEP,
	SYS_FUTEX_WAKEUP,
	SYS_AS_AREA_CREATE,
	SYS_AS_AREA_RESIZE,
	SYS_AS_AREA_DESTROY,
	SYS_IPC_CALL_SYNC_FAST,
	SYS_IPC_CALL_SYNC,
	SYS_IPC_CALL_ASYNC_FAST,
	SYS_IPC_CALL_ASYNC,
	SYS_IPC_ANSWER_FAST,
	SYS_IPC_ANSWER,
	SYS_IPC_FORWARD_FAST,
	SYS_IPC_WAIT,
	SYS_IPC_HANGUP,
	SYS_IPC_REGISTER_IRQ,
	SYS_IPC_UNREGISTER_IRQ,
	SYS_CAP_GRANT,
	SYS_CAP_REVOKE,
	SYS_MAP_PHYSMEM,
	SYS_IOSPACE_ENABLE,
	SYS_PREEMPT_CONTROL,
	SYS_SYSINFO_VALID,
	SYS_SYSINFO_VALUE,
	SYS_DEBUG_ENABLE_CONSOLE,
	SYSCALL_END
} syscall_t;

#ifdef KERNEL

#include <arch/types.h>
#include <typedefs.h>

typedef __native (*syshandler_t)();

extern syshandler_t syscall_table[SYSCALL_END];
extern __native syscall_handler(__native a1, __native a2, __native a3,
				__native a4, __native id);
extern __native sys_tls_set(__native addr);


#endif

#endif
