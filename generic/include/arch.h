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

#ifndef __ARCH_H__
#define __ARCH_H__

#include <arch/types.h>
#include <arch/arch.h>
#include <typedefs.h>

#include <cpu.h>
#include <arch/cpu.h>
#include <arch/asm.h> 

#define CPU			THE->cpu
#define THREAD			THE->thread
#define TASK			THE->task
#define AS			THE->as
#define PREEMPTION_DISABLED	THE->preemption_disabled

/**
 * For each possible kernel stack, structure
 * of the following type will be placed at
 * the base address of the stack.
 */
struct the {
	count_t preemption_disabled;	/**< Preemption disabled counter. */
	thread_t *thread;		/**< Current thread. */
	task_t *task;			/**< Current task. */
	cpu_t *cpu;			/**< Executing cpu. */
	as_t *as;			/**< Current address space. */
};

#define THE		((the_t *)(get_stack_base()))	

extern void the_initialize(the_t *the);
extern void the_copy(the_t *src, the_t *dst);

extern void arch_pre_main(void);
extern void arch_pre_mm_init(void);
extern void arch_post_mm_init(void);
extern void arch_pre_smp_init(void);
extern void arch_post_smp_init(void);
extern void calibrate_delay_loop(void);

extern ipl_t interrupts_disable(void); 
extern ipl_t interrupts_enable(void);
extern void interrupts_restore(ipl_t ipl);
extern ipl_t interrupts_read(void);

#endif
