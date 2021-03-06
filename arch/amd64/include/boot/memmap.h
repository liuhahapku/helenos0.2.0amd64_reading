/*
 * Copyright (C) 2005 Josef Cejka
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

#ifndef __ia32_MEMMAP_H__
#define __ia32_MEMMAP_H__

/* E820h memory range types - other values*/
	/* Free memory */
#define MEMMAP_MEMORY_AVAILABLE	1
	/* Not available for OS */
#define MEMMAP_MEMORY_RESERVED	2 
	/* OS may use it after reading ACPI table */
#define MEMMAP_MEMORY_ACPI	3 
	/* Unusable, required to be saved and restored across an NVS sleep */
#define MEMMAP_MEMORY_NVS	4 
	/* Corrupted memory */
#define MEMMAP_MEMORY_UNUSABLE	5 

	 /* size of one entry */
#define MEMMAP_E820_RECORD_SIZE 20 
	/* maximum entries */
#define MEMMAP_E820_MAX_RECORDS 32 


#ifndef __ASM__

#include <arch/types.h>

struct e820memmap_ {
	__u64 base_address;
	__u64 size;
	__u32 type;
} __attribute__ ((packed));

extern struct e820memmap_ e820table[MEMMAP_E820_MAX_RECORDS];

extern __u8 e820counter; 

extern __u32 e801memorysize; /**< Size of available memory in KB. */

#endif

#endif
