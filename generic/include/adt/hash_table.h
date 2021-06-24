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

#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <adt/list.h>
#include <arch/types.h>
#include <typedefs.h>

/** Hash table structure. */
struct hash_table {
	link_t *entry;
	count_t entries;
	count_t max_keys;
	hash_table_operations_t *op;
};

/** Set of operations for hash table. */
struct hash_table_operations {
	/** Hash function.
	 *
	 * @param key Array of keys needed to compute hash index. All keys must be passed.
	 *
	 * @return Index into hash table.
	 */
	index_t (* hash)(__native key[]);
	
	/** Hash table item comparison function.
	 *
	 * @param key Array of keys that will be compared with item. It is not necessary to pass all keys.
	 *
	 * @return true if the keys match, false otherwise.
	 */
	bool (*compare)(__native key[], count_t keys, link_t *item);

	/** Hash table item removal callback.
	 *
	 * @param item Item that was removed from the hash table.
	 */
	void (*remove_callback)(link_t *item);
};

#define hash_table_get_instance(item, type, member)	list_get_instance((item), type, member)

extern void hash_table_create(hash_table_t *h, count_t m, count_t max_keys, hash_table_operations_t *op);
extern void hash_table_insert(hash_table_t *h, __native key[], link_t *item);
extern link_t *hash_table_find(hash_table_t *h, __native key[]);
extern void hash_table_remove(hash_table_t *h, __native key[], count_t keys);

#endif
