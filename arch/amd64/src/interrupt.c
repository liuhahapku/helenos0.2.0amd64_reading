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

#include <arch/interrupt.h>
#include <print.h>
#include <debug.h>
#include <panic.h>
#include <arch/drivers/i8259.h>
#include <func.h>
#include <cpu.h>
#include <arch/asm.h>
#include <mm/tlb.h>
#include <mm/as.h>
#include <arch.h>
#include <symtab.h>
#include <arch/asm.h>
#include <proc/scheduler.h>
#include <proc/thread.h>
#include <proc/task.h>
#include <synch/spinlock.h>
#include <arch/ddi/ddi.h>
#include <interrupt.h>
#include <ipc/irq.h>

void print_info_errcode(int n, istate_t *istate)
{
	char *symbol;
/*	__u64 *x = &istate->stack[0]; */

	if (!(symbol=get_symtab_entry(istate->rip)))
		symbol = "";

	printf("-----EXCEPTION(%d) OCCURED----- ( %s )\n",n, __FUNCTION__);
	printf("%%rip: %#llX (%s)\n",istate->rip, symbol);
	printf("ERROR_WORD=%#llX\n", istate->error_word);
	printf("%%rcs=%#llX, flags=%#llX, %%cr0=%#llX\n", istate->cs, istate->rflags,read_cr0());
	printf("%%rax=%#llX, %%rcx=%#llX, %%rdx=%#llX\n",istate->rax,istate->rcx,istate->rdx);
	printf("%%rsi=%#llX, %%rdi=%#llX, %%r8 =%#llX\n",istate->rsi,istate->rdi,istate->r8);
	printf("%%r9 =%#llX, %%r10 =%#llX, %%r11=%#llX\n",istate->r9,istate->r10,istate->r11);
#ifdef CONFIG_DEBUG_ALLREGS	
	printf("%%r12=%#llX, %%r13=%#llX, %%r14=%#llX\n",istate->r12,istate->r13,istate->r14);
	printf("%%r15=%#llX, %%rbx=%#llX, %%rbp=%#llX\n",istate->r15,istate->rbx,&istate->rbp);
#endif
	printf("%%rsp=%#llX\n",&istate->stack[0]);
}

/*
 * Interrupt and exception dispatching.
 */

void (* disable_irqs_function)(__u16 irqmask) = NULL;
void (* enable_irqs_function)(__u16 irqmask) = NULL;
void (* eoi_function)(void) = NULL;

void null_interrupt(int n, istate_t *istate)
{
	fault_if_from_uspace(istate, "unserviced interrupt: %d", n);
	print_info_errcode(n, istate);
	panic("unserviced interrupt\n");
}

/** General Protection Fault. */
void gp_fault(int n, istate_t *istate)
{
	if (TASK) {
		count_t ver;

		spinlock_lock(&TASK->lock);
		ver = TASK->arch.iomapver;
		spinlock_unlock(&TASK->lock);

		if (CPU->arch.iomapver_copy != ver) {
			/*
			 * This fault can be caused by an early access
			 * to I/O port because of an out-dated
			 * I/O Permission bitmap installed on CPU.
			 * Install the fresh copy and restart
			 * the instruction.
			 */
			io_perm_bitmap_install();
			return;
		}
		fault_if_from_uspace(istate, "general protection fault");
	}

	print_info_errcode(n, istate);
	panic("general protection fault\n");
}

void ss_fault(int n, istate_t *istate)
{
	fault_if_from_uspace(istate, "stack fault");
	print_info_errcode(n, istate);
	panic("stack fault\n");
}

void nm_fault(int n, istate_t *istate)
{
#ifdef CONFIG_FPU_LAZY     
	scheduler_fpu_lazy_request();
#else
	fault_if_from_uspace(istate, "fpu fault");
	panic("fpu fault");
#endif
}

void tlb_shootdown_ipi(int n, istate_t *istate)
{
	trap_virtual_eoi();
	tlb_shootdown_ipi_recv();
}

void trap_virtual_enable_irqs(__u16 irqmask)
{
	if (enable_irqs_function)
		enable_irqs_function(irqmask);
	else
		panic("no enable_irqs_function\n");
}

void trap_virtual_disable_irqs(__u16 irqmask)
{
	if (disable_irqs_function)
		disable_irqs_function(irqmask);
	else
		panic("no disable_irqs_function\n");
}

void trap_virtual_eoi(void)
{
	if (eoi_function)
		eoi_function();
	else
		panic("no eoi_function\n");

}

static void ipc_int(int n, istate_t *istate)
{
	trap_virtual_eoi();
	ipc_irq_send_notif(n-IVT_IRQBASE);
}


/* Reregister irq to be IPC-ready */
void irq_ipc_bind_arch(__native irq)
{
	if (irq == IRQ_CLK)
		return;
	exc_register(IVT_IRQBASE+irq, "ipc_int", ipc_int);
}
