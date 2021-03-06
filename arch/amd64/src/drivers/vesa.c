/*
 * Copyright (C) 2006 Jakub Vana
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

#ifdef CONFIG_FB

#include <genarch/fb/fb.h>
#include <arch/drivers/vesa.h>
#include <putchar.h>
#include <mm/page.h>
#include <mm/frame.h>
#include <mm/as.h>
#include <arch/mm/page.h>
#include <synch/spinlock.h>
#include <arch/asm.h>
#include <arch/types.h>
#include <typedefs.h>
#include <memstr.h>
#include <bitops.h>

__u32 vesa_ph_addr;
__u16 vesa_width;
__u16 vesa_height;
__u16 vesa_bpp;
__u16 vesa_scanline;

int vesa_present(void)
{
	if (vesa_width != 0xffff)
		return true;
	if (vesa_height != 0xffff)
		return true;
	return false;
}

void vesa_init(void)
{
	fb_init(vesa_ph_addr, vesa_width, vesa_height, vesa_bpp, vesa_scanline);
}

#endif
