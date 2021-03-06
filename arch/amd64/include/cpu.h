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

#ifndef __amd64_CPU_H__
#define __amd64_CPU_H__

#define RFLAGS_IF       (1 << 9)
#define RFLAGS_RF       (1 << 16)

#define EFER_MSR_NUM    0xc0000080
#define AMD_SCE_FLAG    0
#define AMD_LME_FLAG    8
#define AMD_LMA_FLAG    10
#define AMD_FFXSR_FLAG  14
#define AMD_NXE_FLAG    11

/* MSR registers */
#define AMD_MSR_STAR    0xc0000081
#define AMD_MSR_LSTAR   0xc0000082
#define AMD_MSR_SFMASK  0xc0000084
#define AMD_MSR_FS      0xc0000100
#define AMD_MSR_GS      0xc0000101

#ifndef __ASM__

#include <typedefs.h>
#include <arch/pm.h>

struct cpu_arch {
	int vendor;
	int family;
	int model;
	int stepping;
	struct tss *tss;
	
	count_t iomapver_copy;	/** Copy of TASK's I/O Permission bitmap generation count. */
};

struct star_msr {
	
};

struct lstar_msr {
	
};

extern void set_efer_flag(int flag);
extern __u64 read_efer_flag(void);
void cpu_setup_fpu(void);

#endif /* __ASM__ */

#endif
