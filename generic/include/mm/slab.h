/*
 * Copyright (C) 2006 Ondrej Palkovsky
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

#ifndef __SLAB_H__
#define __SLAB_H__

#include <adt/list.h>
#include <synch/spinlock.h>
#include <atomic.h>
#include <mm/frame.h>

/** Minimum size to be allocated by malloc */
#define SLAB_MIN_MALLOC_W 4

/** Maximum size to be allocated by malloc */
#define SLAB_MAX_MALLOC_W 18

/** Initial Magazine size (TODO: dynamically growing magazines) */
#define SLAB_MAG_SIZE  4

/** If object size is less, store control structure inside SLAB */
#define SLAB_INSIDE_SIZE   (PAGE_SIZE >> 3)

/** Maximum wasted space we allow for cache */
#define SLAB_MAX_BADNESS(cache)   ((PAGE_SIZE << (cache)->order) >> 2)

/* slab_reclaim constants */
#define SLAB_RECLAIM_ALL  0x1 /**< Reclaim all possible memory, because
			       *   we are in memory stress */

/* cache_create flags */
#define SLAB_CACHE_NOMAGAZINE 0x1 /**< Do not use per-cpu cache */
#define SLAB_CACHE_SLINSIDE   0x2 /**< Have control structure inside SLAB */
/** We add magazine cache later, if we have this flag */
#define SLAB_CACHE_MAGDEFERRED (0x4 | SLAB_CACHE_NOMAGAZINE)

typedef struct {
	link_t link;
	count_t busy;  /**< Count of full slots in magazine */
	count_t size;  /**< Number of slots in magazine */
	void *objs[0]; /**< Slots in magazine */
}slab_magazine_t;

typedef struct {
	slab_magazine_t *current;
	slab_magazine_t *last;
	SPINLOCK_DECLARE(lock);
}slab_mag_cache_t;


typedef struct {
	char *name;

	link_t link;
	/* Configuration */
	size_t size;      /**< Size of slab position - align_up(sizeof(obj)) */
	int (*constructor)(void *obj, int kmflag);
	int (*destructor)(void *obj);
	int flags;        /**< Flags changing behaviour of cache */

	/* Computed values */
	__u8 order;        /**< Order of frames to be allocated */
	int objects;      /**< Number of objects that fit in */

	/* Statistics */
	atomic_t allocated_slabs;
	atomic_t allocated_objs;
	atomic_t cached_objs;
	atomic_t magazine_counter; /**< How many magazines in magazines list */

	/* Slabs */
	link_t full_slabs;     /**< List of full slabs */
	link_t partial_slabs;  /**< List of partial slabs */
	SPINLOCK_DECLARE(slablock);
	/* Magazines  */
	link_t magazines;      /**< List o full magazines */
	SPINLOCK_DECLARE(maglock);

	/** CPU cache */
	slab_mag_cache_t *mag_cache;
}slab_cache_t;

extern slab_cache_t * slab_cache_create(char *name,
					size_t size,
					size_t align,
					int (*constructor)(void *obj, int kmflag),
					int (*destructor)(void *obj),
					int flags);
extern void slab_cache_destroy(slab_cache_t *cache);

extern void * slab_alloc(slab_cache_t *cache, int flags);
extern void slab_free(slab_cache_t *cache, void *obj);
extern count_t slab_reclaim(int flags);

/** Initialize slab subsytem */
extern void slab_cache_init(void);
extern void slab_enable_cpucache(void);

/* kconsole debug */
extern void slab_print_list(void);

/* malloc support */
extern void * malloc(unsigned int size, int flags);
extern void free(void *obj);
#endif
