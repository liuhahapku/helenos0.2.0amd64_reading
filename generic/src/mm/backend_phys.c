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
 * @file	backend_elf.c
 * @brief	Backend for address space areas backed by continuous physical memory.
 */

#include <debug.h>
#include <arch/types.h>
#include <typedefs.h>
#include <mm/as.h>
#include <mm/frame.h>
#include <mm/slab.h>
#include <memstr.h>
#include <macros.h>
#include <arch.h>
#include <align.h>

static int phys_page_fault(as_area_t *area, __address addr, pf_access_t access);
static void phys_share(as_area_t *area);

mem_backend_t phys_backend = {
	.page_fault = phys_page_fault,
	.frame_free = NULL,
	.share = phys_share
};

/** Service a page fault in the address space area backed by physical memory.
 *
 * The address space area and page tables must be already locked.
 *
 * @param area Pointer to the address space area.
 * @param addr Faulting virtual address.
 * @param access Access mode that caused the fault (i.e. read/write/exec).
 *
 * @return AS_PF_FAULT on failure (i.e. page fault) or AS_PF_OK on success (i.e. serviced).
 */
int phys_page_fault(as_area_t *area, __address addr, pf_access_t access)
{
	__address base = area->backend_data.base;

	if (!as_area_check_access(area, access))
		return AS_PF_FAULT;

	ASSERT(addr - area->base < area->backend_data.frames * FRAME_SIZE);
	page_mapping_insert(AS, addr, base + (addr - area->base), as_area_get_flags(area));
        if (!used_space_insert(area, ALIGN_DOWN(addr, PAGE_SIZE), 1))
                panic("Could not insert used space.\n");

	return AS_PF_OK;
}

/** Share address space area backed by physical memory.
 *
 * Do actually nothing as sharing of address space areas
 * that are backed up by physical memory is very easy.
 * Note that the function must be defined so that
 * as_area_share() will succeed.
 */
void phys_share(as_area_t *area)
{
}
