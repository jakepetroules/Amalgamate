/*
 * Copyright (c) 2014 Petroules Corporation. All rights reserved.
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

#include "dsio.h"
#include "dsrecord.h"
#include "dsstore.h"

void dsstore_header_init(dsstore_header_t *header)
{
    assert(header);
    header->version = 1;
    header->magic = kDSHeaderMagic;
}

static uint32_t pow_uint32_t(uint32_t x, uint32_t y)
{
    if (y == 0)
        return 1;
    const uint32_t result = pow_uint32_t(x, y / 2);
    return (y % 2 == 0) ? (result * result) : (x * result * result);
}

uint32_t dsstore_buddy_allocator_state_block_address_offset(uint32_t a)
{
    return a & ~0x1fu;
}

uint32_t dsstore_buddy_allocator_state_block_address_size(uint32_t a)
{
    return pow_uint32_t(2, a & 0x1f);
}

int amg_read_file(dsstore_header_t *header, FILE *file)
{
    assert(header);
    assert(file);

    if (fread(&header->version, sizeof(header->version), 1, file) != 1)
    {
        fprintf(stderr, "error reading version\n");
        return 1;
    }

    header->version = ntohl(header->version);
    if (header->version != 1)
    {
        fprintf(stderr, "wrong version %u\n", header->version);
        return 1;
    }

    if (fread(&header->magic, sizeof(header->magic), 1, file) != 1)
    {
        fprintf(stderr, "error reading magic\n");
        return 1;
    }

    header->magic = ntohl(header->magic);
    if (header->magic != kDSHeaderMagic)
    {
        fprintf(stderr, "wrong magic '%u', expected '%u'\n", (unsigned int)header->magic, (unsigned int)kDSHeaderMagic);
        return 1;
    }

    if (fread(&header->allocator_offset, sizeof(header->allocator_offset), 1, file) != 1)
    {
        fprintf(stderr, "error reading allocator offset\n");
        return 1;
    }

    header->allocator_offset = ntohl(header->allocator_offset);

    if (fread(&header->allocator_size, sizeof(header->allocator_size), 1, file) != 1)
    {
        fprintf(stderr, "error reading allocator size\n");
        return 1;
    }

    header->allocator_size = ntohl(header->allocator_size);

    if (fread(&header->allocator_offset2, sizeof(header->allocator_offset2), 1, file) != 1)
    {
        fprintf(stderr, "error reading allocator offset copy\n");
        return 1;
    }

    header->allocator_offset2 = ntohl(header->allocator_offset2);
    if (header->allocator_offset != header->allocator_offset2)
    {
        // "invalid storage type bad root address"
        fprintf(stderr, "allocator offset %u does not match check copy %u\n",
                header->allocator_offset, header->allocator_offset2);
        return 1;
    }

    const size_t unknown_size = sizeof(header->padding);
    size_t n;
    if ((n = fread(&header->padding, sizeof(header->padding[0]), unknown_size, file)) != unknown_size)
    {
        fprintf(stderr, "wrong number of unknown header bytes %zu, expected %zu", n, unknown_size);
        return 1;
    }

    return 0;
}

int amg_read_allocator_state(dsstore_buddy_allocator_state_t *allocator_state, FILE *file)
{
    assert(allocator_state);
    assert(file);

    if (fread(&allocator_state->block_count, sizeof(allocator_state->block_count), 1, file) != 1)
    {
        fprintf(stderr, "error reading allocator block count\n");
        return 1;
    }

    allocator_state->block_count = ntohl(allocator_state->block_count);

    if (fread(&allocator_state->unknown, sizeof(allocator_state->unknown), 1, file) != 1)
    {
        fprintf(stderr, "error reading allocator ????\n");
        return 1;
    }

    allocator_state->unknown = ntohl(allocator_state->unknown);

    if (allocator_state->unknown != 0)
    {
        fprintf(stderr, "warning: expected allocator unknown bytes to be 0, got %u\n", allocator_state->unknown);
    }

    // The list of addresses is always padded with zeros to a multiple of 256 entries
    const size_t factor = 256;
    const size_t block_count_to_read = allocator_state->block_count + factor - 1 - (allocator_state->block_count - 1) % factor;

    for (size_t i = 0; i < block_count_to_read; ++i)
    {
        if (fread(&allocator_state->block_addresses[i], sizeof(allocator_state->block_addresses[i]), 1, file) != 1)
        {
            fprintf(stderr, "error reading allocator block address #%zu\n", i);
            return 1;
        }

        allocator_state->block_addresses[i] = ntohl(allocator_state->block_addresses[i]);
    }

    if (fread(&allocator_state->directory_count, sizeof(allocator_state->directory_count), 1, file) != 1)
    {
        fprintf(stderr, "error reading allocator directory count\n");
        return 1;
    }

    allocator_state->directory_count = ntohl(allocator_state->directory_count);

    for (size_t i = 0; i < allocator_state->directory_count; ++i)
    {
        if (fread(&allocator_state->directory_entries[i].count, sizeof(allocator_state->directory_entries[i].count), 1, file) != 1)
        {
            fprintf(stderr, "error reading allocator directory entry #%zu data size\n", i);
            return 1;
        }

        if (fread(&allocator_state->directory_entries[i].bytes, sizeof(allocator_state->directory_entries[i].bytes[0]), allocator_state->directory_entries[i].count, file) != allocator_state->directory_entries[i].count)
        {
            fprintf(stderr, "error reading allocator directory entry #%zu data\n", i);
            return 1;
        }

        if (fread(&allocator_state->directory_entries[i].block_number, sizeof(allocator_state->directory_entries[i].block_number), 1, file) != 1)
        {
            fprintf(stderr, "error reading allocator directory entry #%zu block number\n", i);
            return 1;
        }

        allocator_state->directory_entries[i].block_number = ntohl(allocator_state->directory_entries[i].block_number);
    }

    const size_t free_list_count = sizeof(allocator_state->free_lists) / sizeof(allocator_state->free_lists[0]);
    for (size_t i = 0; i < free_list_count; ++i)
    {
        if (fread(&allocator_state->free_lists[i].count, sizeof(allocator_state->free_lists[i].count), 1, file) != 1)
        {
            fprintf(stderr, "error reading allocator free list #%zu offset count\n", i);
            return 1;
        }

        allocator_state->free_lists[i].count = ntohl(allocator_state->free_lists[i].count);

        for (size_t j = 0; j < allocator_state->free_lists[i].count; ++j)
        {
            if (fread(&allocator_state->free_lists[i].offsets[j], sizeof(allocator_state->free_lists[i].offsets[j]), 1, file) != 1)
            {
                fprintf(stderr, "error reading allocator free list #%zu offset #%zu\n", i, j);
                return 1;
            }

            allocator_state->free_lists[i].offsets[j] = ntohl(allocator_state->free_lists[i].offsets[j]);
        }
    }

    return 0;
}

int amg_read_header_block(dsstore_header_block_t *header_block, FILE *file)
{
    assert(header_block);
    assert(file);

    if (fread_uint32_be(&header_block->root_block_number, file) != 1) {
        fprintf(stderr, "error reading root block number\n");
        return 1;
    }

    if (fread_uint32_be(&header_block->node_levels, file) != 1) {
        fprintf(stderr, "error reading node level count\n");
        return 1;
    }

    if (fread_uint32_be(&header_block->record_count, file) != 1) {
        fprintf(stderr, "error reading record count\n");
        return 1;
    }

    if (fread_uint32_be(&header_block->node_count, file) != 1) {
        fprintf(stderr, "error reading node count\n");
        return 1;
    }

    if (fread_uint32_be(&header_block->tree_node_page_size, file) != 1) {
        fprintf(stderr, "error reading node page size\n");
        return 1;
    }

    if (header_block->tree_node_page_size != dsstore_header_block_tree_node_page_size) {
        fprintf(stderr, "warning: unexpected node page size %u; expected 0x%x\n",
                header_block->tree_node_page_size,
                dsstore_header_block_tree_node_page_size);
        return 1;
    }
    
    return 0;
}
