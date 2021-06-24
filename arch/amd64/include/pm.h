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

#ifndef __amd64_PM_H__
#define __amd64_PM_H__

#ifndef __ASM__
# include <arch/types.h>
# include <typedefs.h>
# include <arch/context.h>
#endif

#define IDT_ITEMS 64
#define GDT_ITEMS 8


#define NULL_DES	0
/* Warning: Do not reorder next items, unless you look into syscall.c!!! */
#define KTEXT_DES	1
#define	KDATA_DES	2
#define UDATA_DES	3
#define UTEXT_DES	4
#define KTEXT32_DES     5
/* EndOfWarning */
#define TSS_DES		6



#ifdef CONFIG_FB

#define VESA_INIT_DES		8
#define VESA_INIT_SEGMENT 0x8000
#undef GDT_ITEMS 
#define GDT_ITEMS 9

#endif /*CONFIG_FB*/



#define gdtselector(des)	((des)<<3)
#define idtselector(des)        ((des)<<4)

#define PL_KERNEL	0
#define PL_USER		3

#define AR_PRESENT	(1<<7)
#define AR_DATA		(2<<3)
#define AR_CODE		(3<<3)
#define AR_WRITABLE	(1<<1)
#define AR_READABLE     (1<<1)
#define AR_TSS		(0x9)
#define AR_INTERRUPT    (0xe)
#define AR_TRAP         (0xf)

#define DPL_KERNEL	(PL_KERNEL<<5)
#define DPL_USER	(PL_USER<<5)

#define TSS_BASIC_SIZE	104
#define TSS_IOMAP_SIZE	(16*1024+1)	/* 16K for bitmap + 1 terminating byte for convenience */

#define IO_PORTS	(64*1024)

#ifndef __ASM__

struct descriptor {
	unsigned limit_0_15: 16;
	unsigned base_0_15: 16;
	unsigned base_16_23: 8;
	unsigned access: 8;
	unsigned limit_16_19: 4;
	unsigned available: 1;
	unsigned longmode: 1;
	unsigned special: 1;
	unsigned granularity : 1;
	unsigned base_24_31: 8;
} __attribute__ ((packed));
typedef struct descriptor descriptor_t;

struct tss_descriptor {
	unsigned limit_0_15: 16;
	unsigned base_0_15: 16;
	unsigned base_16_23: 8;
	unsigned type: 4;
	unsigned : 1;
	unsigned dpl : 2;
	unsigned present : 1;
	unsigned limit_16_19: 4;
	unsigned available: 1;
	unsigned : 2;
	unsigned granularity : 1;
	unsigned base_24_31: 8;	
	unsigned base_32_63 : 32;
	unsigned  : 32;
} __attribute__ ((packed));
typedef struct tss_descriptor tss_descriptor_t;

struct idescriptor {
	unsigned offset_0_15: 16;
	unsigned selector: 16;
	unsigned ist:3;
	unsigned unused: 5;
	unsigned type: 5;
	unsigned dpl: 2;
	unsigned present : 1;
	unsigned offset_16_31: 16;
	unsigned offset_32_63: 32;
	unsigned  : 32;
} __attribute__ ((packed));
typedef struct idescriptor idescriptor_t;

struct ptr_16_64 {
	__u16 limit;
	__u64 base;
} __attribute__ ((packed));
typedef struct ptr_16_64 ptr_16_64_t;

struct ptr_16_32 {
	__u16 limit;
	__u32 base;
} __attribute__ ((packed));
typedef struct ptr_16_32 ptr_16_32_t;

struct tss {
	__u32 reserve1;
	__u64 rsp0;
	__u64 rsp1;
	__u64 rsp2;
	__u64 reserve2;
	__u64 ist1;
	__u64 ist2;
	__u64 ist3;
	__u64 ist4;
	__u64 ist5;
	__u64 ist6;
	__u64 ist7;
	__u64 reserve3;
	__u16 reserve4;
	__u16 iomap_base;
	__u8 iomap[TSS_IOMAP_SIZE];
} __attribute__ ((packed));
typedef struct tss tss_t;

extern tss_t *tss_p;

extern descriptor_t gdt[];
extern idescriptor_t idt[];

extern ptr_16_64_t gdtr;
extern ptr_16_32_t bootstrap_gdtr;
extern ptr_16_32_t protected_ap_gdtr;

extern void pm_init(void);

extern void gdt_tss_setbase(descriptor_t *d, __address base);
extern void gdt_tss_setlimit(descriptor_t *d, __u32 limit);

extern void idt_init(void);
extern void idt_setoffset(idescriptor_t *d, __address offset);

extern void tss_initialize(tss_t *t);

#endif /* __ASM__ */

#endif
