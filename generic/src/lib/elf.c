/*
 * Copyright (C) 2006 Sergey Bondari
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
 * @file	elf.c
 * @brief	Kernel ELF loader.
 */

#include <elf.h>
#include <debug.h>
#include <arch/types.h>
#include <typedefs.h>
#include <mm/as.h>
#include <mm/frame.h>
#include <mm/slab.h>
#include <align.h>
#include <memstr.h>
#include <macros.h>
#include <arch.h>

static char *error_codes[] = {
	"no error",
	"invalid image",
	"address space error",
	"incompatible image",
	"unsupported image type",
	"irrecoverable error"
};

static int segment_header(elf_segment_header_t *entry, elf_header_t *elf, as_t *as);
static int section_header(elf_section_header_t *entry, elf_header_t *elf, as_t *as);
static int load_segment(elf_segment_header_t *entry, elf_header_t *elf, as_t *as);

/** ELF loader
 *
 * @param header Pointer to ELF header in memory
 * @param as Created and properly mapped address space
 * @return EE_OK on success
 */
int elf_load(elf_header_t *header, as_t * as)
{
	int i, rc;

	/* Identify ELF */
	if (header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1 || 
	    header->e_ident[EI_MAG2] != ELFMAG2 || header->e_ident[EI_MAG3] != ELFMAG3) {
		return EE_INVALID;
	}
	
	/* Identify ELF compatibility */
	if (header->e_ident[EI_DATA] != ELF_DATA_ENCODING || header->e_machine != ELF_MACHINE || 
	    header->e_ident[EI_VERSION] != EV_CURRENT || header->e_version != EV_CURRENT ||
	    header->e_ident[EI_CLASS] != ELF_CLASS) {
		return EE_INCOMPATIBLE;
	}

	if (header->e_phentsize != sizeof(elf_segment_header_t))
		return EE_INCOMPATIBLE;

	if (header->e_shentsize != sizeof(elf_section_header_t))
		return EE_INCOMPATIBLE;

	/* Check if the object type is supported. */
	if (header->e_type != ET_EXEC)
		return EE_UNSUPPORTED;

	/* Walk through all segment headers and process them. */
	for (i = 0; i < header->e_phnum; i++) {
		rc = segment_header(&((elf_segment_header_t *)(((__u8 *) header) + header->e_phoff))[i], header, as);
		if (rc != EE_OK)
			return rc;
	}

	/* Inspect all section headers and proccess them. */
	for (i = 0; i < header->e_shnum; i++) {
		rc = section_header(&((elf_section_header_t *)(((__u8 *) header) + header->e_shoff))[i], header, as);
		if (rc != EE_OK)
			return rc;
	}

	return EE_OK;
}

/** Print error message according to error code.
 *
 * @param rc Return code returned by elf_load().
 *
 * @return NULL terminated description of error.
 */
char *elf_error(int rc)
{
	ASSERT(rc < sizeof(error_codes)/sizeof(char *));

	return error_codes[rc];
}

/** Process segment header.
 *
 * @param entry Segment header.
 * @param elf ELF header.
 * @param as Address space into wich the ELF is being loaded.
 *
 * @return EE_OK on success, error code otherwise.
 */
static int segment_header(elf_segment_header_t *entry, elf_header_t *elf, as_t *as)
{
	switch (entry->p_type) {
	    case PT_NULL:
	    case PT_PHDR:
		break;
	    case PT_LOAD:
		return load_segment(entry, elf, as);
		break;
	    case PT_DYNAMIC:
	    case PT_INTERP:
	    case PT_SHLIB:
	    case PT_NOTE:
	    case PT_LOPROC:
	    case PT_HIPROC:
	    default:
		return EE_UNSUPPORTED;
		break;
	}
	return EE_OK;
}

/** Load segment described by program header entry.
 *
 * @param entry Program header entry describing segment to be loaded.
 * @param elf ELF header.
 * @param as Address space into wich the ELF is being loaded.
 *
 * @return EE_OK on success, error code otherwise.
 */
int load_segment(elf_segment_header_t *entry, elf_header_t *elf, as_t *as)
{
	as_area_t *a;
	int flags = 0;
	mem_backend_data_t backend_data;
	
	backend_data.elf = elf;
	backend_data.segment = entry;

	if (entry->p_align > 1) {
		if ((entry->p_offset % entry->p_align) != (entry->p_vaddr % entry->p_align)) {
			return EE_INVALID;
		}
	}

	if (entry->p_flags & PF_X)
		flags |= AS_AREA_EXEC;
	if (entry->p_flags & PF_W)
		flags |= AS_AREA_WRITE;
	if (entry->p_flags & PF_R)
		flags |= AS_AREA_READ;
	flags |= AS_AREA_CACHEABLE;

	/*
	 * Check if the virtual address starts on page boundary.
	 */
	if (ALIGN_UP(entry->p_vaddr, PAGE_SIZE) != entry->p_vaddr)
		return EE_UNSUPPORTED;

	a = as_area_create(as, flags, entry->p_memsz, entry->p_vaddr, AS_AREA_ATTR_NONE,
		&elf_backend, &backend_data);
	if (!a)
		return EE_MEMORY;
	
	/*
	 * The segment will be mapped on demand by elf_page_fault().
	 */

	return EE_OK;
}

/** Process section header.
 *
 * @param entry Segment header.
 * @param elf ELF header.
 * @param as Address space into wich the ELF is being loaded.
 *
 * @return EE_OK on success, error code otherwise.
 */
static int section_header(elf_section_header_t *entry, elf_header_t *elf, as_t *as)
{
	switch (entry->sh_type) {
	    default:
		break;
	}
	
	return EE_OK;
}
