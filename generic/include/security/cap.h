/*
 * Copyright (C) 2006 Jakub Jermar
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
 * @file	cap.h
 * @brief	Capabilities definitions.
 *
 * Capabilities represent virtual rights that entitle their
 * holder to perform certain security sensitive tasks.
 *
 * Each task can have arbitrary combination of the capabilities
 * defined in this file. Therefore, they are required to be powers
 * of two.
 */

#ifndef __CAP_H__
#define __CAP_H__

#include <syscall/sysarg64.h>
#include <arch/types.h>
#include <typedefs.h>

/**
 * CAP_CAP allows its holder to grant/revoke arbitrary
 * privilege to/from other tasks.
 */
#define CAP_CAP			(1<<0)

/**
 * CAP_MEM_MANAGER allows its holder to map physical memory
 * to other tasks.
 */
#define CAP_MEM_MANAGER		(1<<1)

/**
 * CAP_IO_MANAGER allows its holder to access I/O space
 * to other tasks.
 */
#define CAP_IO_MANAGER		(1<<2)

/**
 * CAP_PREEMPT_CONTROL allows its holder to disable/enable preemption.
 */
#define CAP_PREEMPT_CONTROL	(1<<3)

/**
 * CAP_IRQ_REG entitles its holder to register IRQ handlers.
 */
#define CAP_IRQ_REG		(1<<4) 

typedef __u32 cap_t;

extern void cap_set(task_t *t, cap_t caps);
extern cap_t cap_get(task_t *t);

extern __native sys_cap_grant(sysarg64_t *uspace_taskid_arg, cap_t caps);
extern __native sys_cap_revoke(sysarg64_t *uspace_taskid_arg, cap_t caps);

#endif
