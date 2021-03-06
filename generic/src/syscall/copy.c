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
 * @file	copy.c
 * @brief	Copying between kernel and userspace.
 *
 * This file contains sanitized functions for copying data
 * between kernel and userspace.
 */

#include <syscall/copy.h>
#include <proc/thread.h>
#include <mm/as.h>
#include <macros.h>
#include <arch.h>
#include <errno.h>
#include <typedefs.h>

/** Copy data from userspace to kernel.
 *
 * Provisions are made to return value even after page fault.
 *
 * This function can be called only from syscall.
 *
 * @param dst Destination kernel address.
 * @param uspace_src Source userspace address.
 * @param size Size of the data to be copied.
 *
 * @return 0 on success or error code from @ref errno.h.
 */
int copy_from_uspace(void *dst, const void *uspace_src, size_t size)
{
	ipl_t ipl;
	int rc;
	
	ASSERT(THREAD);
	ASSERT(!THREAD->in_copy_from_uspace);
	
	if (!KERNEL_ADDRESS_SPACE_SHADOWED) {
		if (overlaps((__address) uspace_src, size,
			KERNEL_ADDRESS_SPACE_START, KERNEL_ADDRESS_SPACE_END-KERNEL_ADDRESS_SPACE_START)) {
			/*
			 * The userspace source block conflicts with kernel address space.
			 */
			return EPERM;
		}
	}
	
	ipl = interrupts_disable();
	THREAD->in_copy_from_uspace = true;
	
	rc = memcpy_from_uspace(dst, uspace_src, size);

	THREAD->in_copy_from_uspace = false;

	interrupts_restore(ipl);
	return !rc ? EPERM : 0;
}

/** Copy data from kernel to userspace.
 *
 * Provisions are made to return value even after page fault.
 *
 * This function can be called only from syscall.
 *
 * @param uspace_dst Destination userspace address.
 * @param uspace_src Source kernel address.
 * @param size Size of the data to be copied.
 *
 * @return 0 on success or error code from @ref errno.h.
 */
int copy_to_uspace(void *uspace_dst, const void *src, size_t size)
{
	ipl_t ipl;
	int rc;
	
	ASSERT(THREAD);
	ASSERT(!THREAD->in_copy_from_uspace);
	
	if (!KERNEL_ADDRESS_SPACE_SHADOWED) {
		if (overlaps((__address) uspace_dst, size,
			KERNEL_ADDRESS_SPACE_START, KERNEL_ADDRESS_SPACE_END-KERNEL_ADDRESS_SPACE_START)) {
			/*
			 * The userspace destination block conflicts with kernel address space.
			 */
			return EPERM;
		}
	}
	
	ipl = interrupts_disable();
	THREAD->in_copy_from_uspace = true;
	
	rc = memcpy_to_uspace(uspace_dst, src, size);

	THREAD->in_copy_from_uspace = false;

	interrupts_restore(ipl);
	return !rc ? EPERM : 0;
}
