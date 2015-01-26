/*
 * Copyright (c) 2015 Jake Petroules. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMALGAMATE_DSSTORE_P_H
#define AMALGAMATE_DSSTORE_P_H

#include "dsstore.h"

typedef struct {
    uint32_t version;
    FourCharCode magic;

    uint32_t allocator_offset; // "rootAddress"
    uint32_t allocator_size;
    uint32_t allocator_offset_check; // "checkAddr"
    uint8_t padding[16];
} dsstore_header_t;

typedef struct {
    uint32_t block_count;
    uint32_t unknown;
    uint32_t block_addresses[256 * sizeof(uint32_t)];
    uint32_t directory_count;
    struct {
        uint8_t count;
        uint8_t bytes[255];
        uint32_t block_number;
    } directory_entries[256];
    struct {
        uint32_t count;
        uint32_t offsets[256];
    } free_lists[32];
} dsstore_buddy_allocator_state_t;

AMG_EXPORT AMG_EXTERN uint32_t dsstore_buddy_allocator_state_block_address_offset(uint32_t a);
AMG_EXPORT AMG_EXTERN uint32_t dsstore_buddy_allocator_state_block_address_size(uint32_t a);

typedef struct {
    uint32_t root_block_number;
    uint32_t node_levels;
    uint32_t record_count;
    uint32_t node_count;
    uint32_t tree_node_page_size; // ??? always dsstore_header_block_tree_node_page_size
} dsstore_header_block_t;

static const uint32_t dsstore_header_block_tree_node_page_size = 0x1000;

AMG_EXPORT AMG_EXTERN int dsstore_header_fread(dsstore_header_t *header, FILE *file);
AMG_EXPORT AMG_EXTERN int dsstore_buddy_allocator_state_fread(dsstore_buddy_allocator_state_t *allocator_state, FILE *file);
AMG_EXPORT AMG_EXTERN int dsstore_header_block_fread(dsstore_header_block_t *header_block, FILE *file);

// Debugging

#ifdef __cplusplus
AMG_EXPORT AMG_EXTERN int ds_store_enum_blocks_core(dsstore_buddy_allocator_state_t *allocator, dsstore_header_block_t *header_block, uint32_t block_number, const std::function<void(ds_record_t *)> &record_func, FILE *file);
#endif

AMG_EXPORT AMG_EXTERN int ds_store_enum_blocks(dsstore_buddy_allocator_state_t *allocator, dsstore_header_block_t *header_block, uint32_t block_number, ds_store_record_func_t record_func, FILE *file);
AMG_EXPORT AMG_EXTERN void dsstore_header_dump(dsstore_header_t *header);
AMG_EXPORT AMG_EXTERN void dsstore_buddy_allocator_state_dump(dsstore_buddy_allocator_state_t *allocator_state);
AMG_EXPORT AMG_EXTERN void dsstore_header_block_dump(dsstore_header_block_t *header_block);

#endif // AMALGAMATE_DSSTORE_P_H
