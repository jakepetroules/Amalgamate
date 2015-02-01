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
#include "dsstore_p.h"

struct _ds_store
{
    _ds_store();
    FILE *file;
    dsstore_header_t header;
    dsstore_buddy_allocator_state_t allocator;
    dsstore_header_block_t header_block;
    char padding[4];

private:
    _ds_store(const _ds_store &);
    _ds_store &operator=(const _ds_store &);
};

_ds_store::_ds_store()
    : file()
{
    header.version = 1;
    header.magic = kDSHeaderMagic;
    header.allocator_offset = 0;
    header.allocator_size = 0;
    header.allocator_offset_check = 0;
}

ds_store_t *ds_store_fread(FILE *file)
{
    ds_store_t *store = ds_store_create();
    store->file = file;

    if (dsstore_header_fread(&store->header, file) != 0) {
        ds_store_free(store);
        return nullptr;
    }

    if (fseek(file, static_cast<long>(sizeof(store->header.version) + store->header.allocator_offset), SEEK_SET) != 0) {
        fprintf(stderr, "could not seek to buddy allocator offset\n");
        return nullptr;
    }

    if (dsstore_buddy_allocator_state_fread(&store->allocator, file) != 0) {
        ds_store_free(store);
        return nullptr;
    }

    // Find the DSDB directory entry...
    uint32_t header_block_number = UINT32_MAX;
    const uint32_t dsdb = htonl(FOUR_CHAR_CODE('DSDB'));
    for (size_t i = 0; i < store->allocator.directory_count; ++i) {
        if (memcmp(store->allocator.directory_entries[i].bytes, &dsdb, sizeof(uint32_t)) == 0) {
            header_block_number = store->allocator.directory_entries[i].block_number;
            break;
        }
    }

    if (header_block_number == UINT32_MAX) {
        fprintf(stderr, "could not find the DSDB directory entry\n");
        return nullptr;
    }

    if (header_block_number >= store->allocator.block_count) {
        fprintf(stderr, "header block number out of range\n");
        return nullptr;
    }

    // Look up the offset of the header block
    const uint32_t header_block_addr = store->allocator.block_addresses[header_block_number];
    const uint32_t header_block_offset = dsstore_buddy_allocator_state_block_address_offset(header_block_addr);

    if (fseek(file, static_cast<long>(sizeof(store->header.version) + header_block_offset), SEEK_SET) != 0) {
        fprintf(stderr, "could not seek to header block offset\n");
        return nullptr;
    }

    if (dsstore_header_block_fread(&store->header_block, file) != 0) {
        ds_store_free(store);
        return nullptr;
    }

    return store;
}

ds_store_t *ds_store_create(void)
{
    return new _ds_store();
}

void ds_store_free(ds_store_t *store)
{
    delete store;
}

int ds_store_enum_blocks_core(dsstore_buddy_allocator_state_t *allocator, dsstore_header_block_t *header_block, uint32_t block_number, const std::function<void(ds_record_t *)> &record_func, FILE *file)
{

    int ret = 0, status = 0;

    if (header_block->root_block_number >= allocator->block_count) {
        fprintf(stderr, "root node block number out of range\n");
        ret = 1;
        return ret;
    }

    // Look up the offset of the root node
    const uint32_t root_block_addr = allocator->block_addresses[block_number];
    const uint32_t root_block_offset = dsstore_buddy_allocator_state_block_address_offset(root_block_addr);

    off_t stream_offset = ftell(file);
    dsstore_header_t header; // for sizeof
    if (fseek(file, static_cast<long>(sizeof(header.version) + root_block_offset), SEEK_SET) != 0) {
        fprintf(stderr, "could not seek to root block (records) offset\n");
        ret = 1;
        goto cleanup;
    }

    uint32_t node_type;
    if (fread_uint32_be(&node_type, file) != 1) {
        fprintf(stderr, "error reading node type\n");
        ret = 1;
        goto cleanup;
    }

    uint32_t record_count;
    if (fread_uint32_be(&record_count, file) != 1) {
        fprintf(stderr, "error reading record count\n");
        ret = 1;
        goto cleanup;
    }

    //fprintf(stdout, "record count: %u\n", record_count);

    // Leaf node
    if (node_type == 0) {
        for (size_t i = 0; i < record_count; ++i) {
            ds_record_t *record = ds_record_create();
            if ((status = ds_record_fread(record, file)) != 0) {
                ds_record_free(record);
                ret = 1;
                goto cleanup;
            }

            if (record_func) {
                record_func(record);
            }

            ds_record_free(record);
        }
    } else {
        // Internal node...
        for (size_t i = 0; i < record_count; ++i) {
            if (fread_uint32_be(&block_number, file) != 1) {
                fprintf(stderr, "error reading internal node block number\n");
                ret = 1;
                goto cleanup;
            }

            if (ds_store_enum_blocks_core(allocator, header_block, block_number, record_func, file) != 0) {
                ret = 1;
                goto cleanup;
            }

            ds_record_t *record = ds_record_create();
            if ((status = ds_record_fread(record, file)) != 0) {
                ds_record_free(record);
                ret = 1;
                goto cleanup;
            }

            if (record_func) {
                record_func(record);
            }

            ds_record_free(record);
        }

        if (ds_store_enum_blocks_core(allocator, header_block, node_type, record_func, file) != 0) {
            ret = 1;
            goto cleanup;
        }
    }

    // Restore stream offset
    if (fseek(file, static_cast<long>(stream_offset), SEEK_SET) != 0) {
        fprintf(stderr, "failed to restore original stream offset after reading block\n");
        ret = 1;
        goto cleanup;
    }

cleanup:
    return ret;
}

int ds_store_enum_records(ds_store_t *store, ds_store_record_func_t func)
{
    return ds_store_enum_blocks_core(&store->allocator, &store->header_block, store->header_block.root_block_number, func, store->file);
}

int ds_store_enum_records_core(ds_store_t *store, const std::function<void(ds_record_t *)> &func)
{
    return ds_store_enum_blocks_core(&store->allocator, &store->header_block, store->header_block.root_block_number, func, store->file);
}

int ds_store_enum_blocks(dsstore_buddy_allocator_state_t *allocator, dsstore_header_block_t *header_block, uint32_t block_number, ds_store_record_func_t record_func, FILE *file)
{
    return ds_store_enum_blocks_core(allocator, header_block, block_number, record_func, file);
}

void dsstore_header_dump(dsstore_header_t *header)
{
    fprintf(stdout, "version: %u\n", header->version);
    const uint32_t magic_n = htonl(header->magic);
    fprintf(stdout, "magic: '%.4s' (%u)\n",
            reinterpret_cast<const char *>(&magic_n),
            static_cast<unsigned int>(header->magic));
    fprintf(stdout, "allocator offset: %u\n", header->allocator_offset);
    fprintf(stdout, "allocator size: %u\n", header->allocator_size);
    fprintf(stdout, "allocator offset copy: %u\n", header->allocator_offset_check);
    fprintf(stdout, "padding: 0x");
    for (size_t i = 0; i < sizeof(header->padding); ++i)
        fprintf(stdout, "%02x", header->padding[i]);
    fprintf(stdout, "\n");
}

void dsstore_buddy_allocator_state_dump(dsstore_buddy_allocator_state_t *allocator_state)
{
    fprintf(stdout, "allocator block count: %u\n", allocator_state->block_count);
    fprintf(stdout, "allocator unknown?: %u\n", allocator_state->unknown);
    fprintf(stdout, "allocator block addresses:");
    for (size_t i = 0; i < allocator_state->block_count; ++i) {
        // Each address is a packed offset and size...
        const uint32_t addr = allocator_state->block_addresses[i];
        const uint32_t offset = dsstore_buddy_allocator_state_block_address_offset(addr);
        const uint32_t size = dsstore_buddy_allocator_state_block_address_size(addr);
        fprintf(stdout, " {%u, %u}", offset, size);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "allocator directory entry count: %u\n", allocator_state->directory_count);
    for (size_t i = 0; i < allocator_state->directory_count; ++i) {
        fprintf(stdout, "\tcount: %u\n", allocator_state->directory_entries[i].count);
        fprintf(stdout, "\tname: %.255s\n", allocator_state->directory_entries[i].bytes);
        fprintf(stdout, "\tblock number: %u\n", allocator_state->directory_entries[i].block_number);
    }

    const size_t free_list_count = sizeof(allocator_state->free_lists) / sizeof(allocator_state->free_lists[0]);
    for (size_t i = 0; i < free_list_count; ++i) {
        fprintf(stdout, "free list #%zu:", i);
        for (size_t j = 0; j < allocator_state->free_lists[i].count; ++j)
            fprintf(stdout, " %u", allocator_state->free_lists[i].offsets[j]);
        fprintf(stdout, "\n");
    }
}

void dsstore_header_block_dump(dsstore_header_block_t *header_block)
{
    fprintf(stdout, "root block number: %u\n", header_block->root_block_number);
    fprintf(stdout, "node level count: %u\n", header_block->node_levels);
    fprintf(stdout, "record count: %u\n", header_block->record_count);
    fprintf(stdout, "node count: %u\n", header_block->node_count);
    fprintf(stdout, "tree node page size: %u\n", header_block->tree_node_page_size);
}

void ds_store_dump_header(ds_store_t *store)
{
    dsstore_header_dump(&store->header);
}

void ds_store_dump_allocator_state(ds_store_t *store)
{
    dsstore_buddy_allocator_state_dump(&store->allocator);
}

void dsstore_header_dumpblock(ds_store_t *store)
{
    dsstore_header_block_dump(&store->header_block);
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
    return pow_uint32_t(2, a & 0x1fu);
}

int dsstore_header_fread(dsstore_header_t *header, FILE *file)
{
    assert(header);
    assert(file);

    if (fread_uint32_be(&header->version, file) != 1) {
        fprintf(stderr, "error reading version\n");
        return 1;
    }

    if (header->version != 1) {
        fprintf(stderr, "wrong version %u\n", header->version);
        return 1;
    }

    uint32_t magic;
    if (fread_uint32_be(&magic, file) != 1) {
        fprintf(stderr, "error reading magic\n");
        return 1;
    }

    header->magic = static_cast<FourCharCode>(magic);

    if (header->magic != kDSHeaderMagic) {
        fprintf(stderr, "wrong magic '%u', expected '%u'\n",
                static_cast<unsigned int>(header->magic),
                static_cast<unsigned int>(kDSHeaderMagic));
        return 1;
    }

    if (fread_uint32_be(&header->allocator_offset, file) != 1) {
        fprintf(stderr, "error reading allocator offset\n");
        return 1;
    }

    if (fread_uint32_be(&header->allocator_size, file) != 1) {
        fprintf(stderr, "error reading allocator size\n");
        return 1;
    }

    if (fread_uint32_be(&header->allocator_offset_check, file) != 1) {
        fprintf(stderr, "error reading allocator offset copy\n");
        return 1;
    }

    if (header->allocator_offset != header->allocator_offset_check) {
        // "invalid storage type bad root address"
        fprintf(stderr, "allocator offset %u does not match check copy %u\n",
                header->allocator_offset, header->allocator_offset_check);
        return 1;
    }

    const size_t unknown_size = sizeof(header->padding);
    size_t n;
    if ((n = fread(&header->padding, sizeof(header->padding[0]), unknown_size, file)) != unknown_size) {
        fprintf(stderr, "wrong number of unknown header bytes %zu, expected %zu", n, unknown_size);
        return 1;
    }

    return 0;
}

int dsstore_buddy_allocator_state_fread(dsstore_buddy_allocator_state_t *allocator_state, FILE *file)
{
    assert(allocator_state);
    assert(file);

    if (fread_uint32_be(&allocator_state->block_count, file) != 1)
    {
        fprintf(stderr, "error reading allocator block count\n");
        return 1;
    }

    if (fread_uint32_be(&allocator_state->unknown, file) != 1)
    {
        fprintf(stderr, "error reading allocator ????\n");
        return 1;
    }

    if (allocator_state->unknown != 0)
    {
        fprintf(stderr, "warning: expected allocator unknown bytes to be 0, got %u\n", allocator_state->unknown);
    }

    // The list of addresses is always padded with zeros to a multiple of 256 entries
    const size_t factor = 256;
    const size_t block_count_to_read = allocator_state->block_count + factor - 1 - (allocator_state->block_count - 1) % factor;

    for (size_t i = 0; i < block_count_to_read; ++i)
    {
        if (fread_uint32_be(&allocator_state->block_addresses[i], file) != 1)
        {
            fprintf(stderr, "error reading allocator block address #%zu\n", i);
            return 1;
        }
    }

    if (fread_uint32_be(&allocator_state->directory_count, file) != 1) {
        fprintf(stderr, "error reading allocator directory count\n");
        return 1;
    }

    for (size_t i = 0; i < allocator_state->directory_count; ++i) {
        if (fread_uint8(&allocator_state->directory_entries[i].count, file) != 1) {
            fprintf(stderr, "error reading allocator directory entry #%zu data size\n", i);
            return 1;
        }

        if (fread(&allocator_state->directory_entries[i].bytes, sizeof(allocator_state->directory_entries[i].bytes[0]), allocator_state->directory_entries[i].count, file) != allocator_state->directory_entries[i].count) {
            fprintf(stderr, "error reading allocator directory entry #%zu data\n", i);
            return 1;
        }

        if (fread_uint32_be(&allocator_state->directory_entries[i].block_number, file) != 1) {
            fprintf(stderr, "error reading allocator directory entry #%zu block number\n", i);
            return 1;
        }
    }

    const size_t free_list_count = sizeof(allocator_state->free_lists) / sizeof(allocator_state->free_lists[0]);
    for (size_t i = 0; i < free_list_count; ++i) {
        if (fread_uint32_be(&allocator_state->free_lists[i].count, file) != 1) {
            fprintf(stderr, "error reading allocator free list #%zu offset count\n", i);
            return 1;
        }

        for (size_t j = 0; j < allocator_state->free_lists[i].count; ++j) {
            if (fread_uint32_be(&allocator_state->free_lists[i].offsets[j], file) != 1) {
                fprintf(stderr, "error reading allocator free list #%zu offset #%zu\n", i, j);
                return 1;
            }
        }
    }

    return 0;
}

int dsstore_header_block_fread(dsstore_header_block_t *header_block, FILE *file)
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
