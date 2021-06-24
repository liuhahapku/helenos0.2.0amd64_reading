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

#ifndef __AS_H__
#define __AS_H__

/** Address space area flags. */
#define AS_AREA_READ		1
#define AS_AREA_WRITE		2
#define AS_AREA_EXEC		4
#define AS_AREA_CACHEABLE	8

#ifdef KERNEL

#include <arch/mm/page.h>
#include <arch/mm/as.h>
#include <arch/mm/asid.h>
#include <arch/types.h>
#include <typedefs.h>
#include <synch/spinlock.h>
#include <synch/mutex.h>
#include <adt/list.h>
#include <adt/btree.h>
#include <elf.h>

/** Defined to be true if user address space and kernel address space shadow each other. */
#define KERNEL_ADDRESS_SPACE_SHADOWED	KERNEL_ADDRESS_SPACE_SHADOWED_ARCH

#define KERNEL_ADDRESS_SPACE_START	KERNEL_ADDRESS_SPACE_START_ARCH
#define KERNEL_ADDRESS_SPACE_END	KERNEL_ADDRESS_SPACE_END_ARCH
#define USER_ADDRESS_SPACE_START	USER_ADDRESS_SPACE_START_ARCH
#define USER_ADDRESS_SPACE_END		USER_ADDRESS_SPACE_END_ARCH

#define IS_KA(addr)	((addr)>=KERNEL_ADDRESS_SPACE_START && (addr)<=KERNEL_ADDRESS_SPACE_END)

#define USTACK_ADDRESS	USTACK_ADDRESS_ARCH

#define FLAG_AS_KERNEL	    (1 << 0)	/**< Kernel address space. */

/** Address space structure.
 *
 * as_t contains the list of as_areas of userspace accessible
 * pages for one or more tasks. Ranges of kernel memory pages are not
 * supposed to figure in the list as they are shared by all tasks and
 * set up during system initialization.
 */
struct as {
	/** Protected by asidlock. */
	link_t inactive_as_with_asid_link;

	mutex_t lock;

	/** Number of references (i.e tasks that reference this as). */
	count_t refcount;

	/** Number of processors on wich is this address space active. */
	count_t cpu_refcount;

	/** B+tree of address space areas. */
	btree_t as_area_btree;

	/** Page table pointer. Constant on architectures that use global page hash table. */
	pte_t *page_table;

	/** Address space identifier. Constant on architectures that do not support ASIDs.*/
	asid_t asid;
};

struct as_operations {
	pte_t *(* page_table_create)(int flags);
	void (* page_table_destroy)(pte_t *page_table);
	void (* page_table_lock)(as_t *as, bool lock);
	void (* page_table_unlock)(as_t *as, bool unlock);
};
typedef struct as_operations as_operations_t;

/** Address space area attributes. */
#define AS_AREA_ATTR_NONE	0
#define AS_AREA_ATTR_PARTIAL	1	/**< Not fully initialized area. */

#define AS_PF_FAULT		0	/**< The page fault was not resolved by as_page_fault(). */
#define AS_PF_OK		1	/**< The page fault was resolved by as_page_fault(). */
#define AS_PF_DEFER		2	/**< The page fault was caused by memcpy_from_uspace()
					     or memcpy_to_uspace(). */

/** This structure contains information associated with the shared address space area. */
typedef struct {
	mutex_t lock;		/**< This lock must be acquired only when the as_area lock is held. */
	count_t refcount;	/**< This structure can be deallocated if refcount drops to 0. */
	btree_t pagemap;	/**< B+tree containing complete map of anonymous pages of the shared area. */
} share_info_t;

/** Address space area backend structure. */
typedef struct {
	int (* page_fault)(as_area_t *area, __address addr, pf_access_t access);
	void (* frame_free)(as_area_t *area, __address page, __address frame);
	void (* share)(as_area_t *area);
} mem_backend_t;

/** Backend data stored in address space area. */
typedef union {
	struct {	/**< elf_backend members */
		elf_header_t *elf;
		elf_segment_header_t *segment;
	};
	struct {	/**< phys_backend members */
		__address base;
		count_t frames;
	};
} mem_backend_data_t;

/** Address space area structure.
 *
 * Each as_area_t structure describes one contiguous area of virtual memory.
 * In the future, it should not be difficult to support shared areas.
 */
struct as_area {
	mutex_t lock;
	as_t *as;		/**< Containing address space. */
	int flags;		/**< Flags related to the memory represented by the address space area. */
	int attributes;		/**< Attributes related to the address space area itself. */
	count_t pages;		/**< Size of this area in multiples of PAGE_SIZE. */
	__address base;		/**< Base address of this area. */
	btree_t used_space;	/**< Map of used space. */
	share_info_t *sh_info;	/**< If the address space area has been shared, this pointer will
			     	     reference the share info structure. */
	mem_backend_t *backend;	/**< Memory backend backing this address space area. */

	/** Data to be used by the backend. */
	mem_backend_data_t backend_data;
};

extern as_t *AS_KERNEL;
extern as_operations_t *as_operations;

extern spinlock_t inactive_as_with_asid_lock;
extern link_t inactive_as_with_asid_head;

extern void as_init(void);

extern as_t *as_create(int flags);
extern void as_destroy(as_t *as);
extern void as_switch(as_t *old, as_t *new);
extern int as_page_fault(__address page, pf_access_t access, istate_t *istate);

extern as_area_t *as_area_create(as_t *as, int flags, size_t size, __address base, int attrs,
	mem_backend_t *backend, mem_backend_data_t *backend_data);
extern int as_area_destroy(as_t *as, __address address);	
extern int as_area_resize(as_t *as, __address address, size_t size, int flags);
int as_area_share(as_t *src_as, __address src_base, size_t acc_size,
		  as_t *dst_as, __address dst_base, int dst_flags_mask);

extern int as_area_get_flags(as_area_t *area);
extern bool as_area_check_access(as_area_t *area, pf_access_t access);
extern size_t as_get_size(__address base);
extern int used_space_insert(as_area_t *a, __address page, count_t count);
extern int used_space_remove(as_area_t *a, __address page, count_t count);

/* Interface to be implemented by architectures. */
#ifndef as_install_arch
extern void as_install_arch(as_t *as);
#endif /* !def as_install_arch */

/* Backend declarations. */
extern mem_backend_t anon_backend;
extern mem_backend_t elf_backend;
extern mem_backend_t phys_backend;

/* Address space area related syscalls. */
extern __native sys_as_area_create(__address address, size_t size, int flags);
extern __native sys_as_area_resize(__address address, size_t size, int flags);
extern __native sys_as_area_destroy(__address address);

#endif /* KERNEL */

#endif
