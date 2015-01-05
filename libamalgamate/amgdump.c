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

#include "amgdump.h"
#include "dsio.h"
#include "dsrecord.h"

int amg_dump_file(const char *filename)
{
    assert(filename);

    int ret = 0;
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "error opening file %s\n", filename);
        ret = 1;
        goto cleanup;
    }

    dsstore_header_t header;
    int status = amg_read_file(&header, file);
    if (status != 0)
    {
        ret = 1;
        goto cleanup;
    }

    fprintf(stdout, "version: %u\n", header.version);
    const uint32_t magic_n = htonl(header.magic);
    fprintf(stdout, "magic: '%.4s' (%u)\n", (const char *)&magic_n, (unsigned int)header.magic);
    fprintf(stdout, "allocator offset: %u\n", header.allocator_offset);
    fprintf(stdout, "allocator size: %u\n", header.allocator_size);
    fprintf(stdout, "allocator offset copy: %u\n", header.allocator_offset2);
    fprintf(stdout, "padding: 0x");
    for (size_t i = 0; i < sizeof(header.padding); ++i)
        fprintf(stdout, "%02x", header.padding[i]);
    fprintf(stdout, "\n");

    if (fseek(file, (long)(sizeof(header.version) + header.allocator_offset), SEEK_SET) != 0) {
        fprintf(stderr, "could not seek to buddy allocator offset\n");
        ret = 1;
        goto cleanup;
    }

    dsstore_buddy_allocator_state_t allocator_state;
    status = amg_read_allocator_state(&allocator_state, file);
    if (status != 0)
    {
        ret = 1;
        goto cleanup;
    }

    status = amg_dump_allocator_state(&allocator_state, file);
    if (status != 0)
    {
        ret = 1;
        goto cleanup;
    }

    // Find the DSDB directory entry...
    uint32_t header_block_number = UINT32_MAX;
    const uint32_t dsdb = htonl(FOUR_CHAR_CODE('DSDB'));
    for (size_t i = 0; i < allocator_state.directory_count; ++i) {
        if (memcmp(allocator_state.directory_entries[i].bytes, &dsdb, sizeof(uint32_t)) == 0) {
            header_block_number = allocator_state.directory_entries[i].block_number;
            break;
        }
    }

    if (header_block_number == UINT32_MAX) {
        fprintf(stderr, "could not find the DSDB directory entry\n");
        ret = 1;
        goto cleanup;
    }

    if (header_block_number >= allocator_state.block_count) {
        fprintf(stderr, "header block number out of range\n");
        ret = 1;
        goto cleanup;
    }

    // Look up the offset of the header block
    const uint32_t header_block_addr = allocator_state.block_addresses[header_block_number];
    const uint32_t header_block_offset = dsstore_buddy_allocator_state_block_address_offset(header_block_addr);
    const uint32_t header_block_size = dsstore_buddy_allocator_state_block_address_size(header_block_addr);

    if (fseek(file, (long)(sizeof(header.version) + header_block_offset), SEEK_SET) != 0) {
        fprintf(stderr, "could not seek to header block offset\n");
        ret = 1;
        goto cleanup;
    }

    dsstore_header_block_t header_block;
    status = amg_read_header_block(&header_block, file);
    if (status != 0) {
        ret = 1;
        goto cleanup;
    }

    status = amg_dump_header_block(&header_block, file);
    if (status != 0) {
        ret = 1;
        goto cleanup;
    }

    // Dump the root block...
    amg_dump_block(&allocator_state, &header_block, header_block.root_block_number, file);

cleanup:
    fclose(file);
    return ret;
}

int amg_dump_block(dsstore_buddy_allocator_state_t *allocator_state,
                   dsstore_header_block_t *header_block,
                   uint32_t block_number, FILE *file)
{
    assert(allocator_state);
    assert(header_block);

    int ret = 0, status = 0;

    if (header_block->root_block_number >= allocator_state->block_count) {
        fprintf(stderr, "root node block number out of range\n");
        ret = 1;
        goto cleanup;
    }

    // Look up the offset of the root node
    const uint32_t root_block_addr = allocator_state->block_addresses[block_number];
    const uint32_t root_block_offset = dsstore_buddy_allocator_state_block_address_offset(root_block_addr);
    const uint32_t root_block_size = dsstore_buddy_allocator_state_block_address_size(root_block_addr);

    off_t stream_offset = ftell(file);
    if (fseek(file, (long)(/*sizeof(header.version)*/ 4 + root_block_offset), SEEK_SET) != 0) {
        fprintf(stderr, "could not seek to root block (records) offset\n");
        ret = 1;
        goto cleanup;
    }

    uint32_t node_type;
    if (fread_uint32_be(&node_type, file) != 1)
    {
        fprintf(stderr, "error reading node type\n");
        ret = 1;
        goto cleanup;
    }

    uint32_t record_count;
    if (fread_uint32_be(&record_count, file) != 1)
    {
        fprintf(stderr, "error reading record count\n");
        ret = 1;
        goto cleanup;
    }

    fprintf(stdout, "record count: %u\n", record_count);

    // Leaf node
    if (node_type == 0) {
        for (size_t i = 0; i < record_count; ++i) {
            if ((status = amg_dump_record(file)) != 0) {
                ret = 1;
                goto cleanup;
            }
        }
    } else {
        // Internal node...
        for (size_t i = 0; i < record_count; ++i) {
            if (fread_uint32_be(&block_number, file) != 1) {
                fprintf(stderr, "error reading internal node block number\n");
                ret = 1;
                goto cleanup;
            }

            if (amg_dump_block(allocator_state, header_block, block_number, file) != 0) {
                ret = 1;
                goto cleanup;
            }

            if ((status = amg_dump_record(file)) != 0) {
                ret = 1;
                goto cleanup;
            }
        }

        if (amg_dump_block(allocator_state, header_block, node_type, file) != 0) {
            ret = 1;
            goto cleanup;
        }
    }

    // Restore stream offset
    if (fseek(file, (long)stream_offset, SEEK_SET) != 0) {
        fprintf(stderr, "failed to restore original stream offset after reading block\n");
        ret = 1;
        goto cleanup;
    }

cleanup:
    return ret;
}

int amg_dump_allocator_state(dsstore_buddy_allocator_state_t *allocator_state, FILE *file)
{
    (void)file;

    fprintf(stdout, "\n");

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

    return 0;
}

int amg_dump_header_block(dsstore_header_block_t *header_block, FILE *file)
{
    (void)file;

    fprintf(stdout, "\n");

    fprintf(stdout, "root block number: %u\n", header_block->root_block_number);
    fprintf(stdout, "node level count: %u\n", header_block->node_levels);
    fprintf(stdout, "record count: %u\n", header_block->record_count);
    fprintf(stdout, "node count: %u\n", header_block->node_count);
    fprintf(stdout, "tree node page size: %u\n", header_block->tree_node_page_size);

    return 0;
}

static inline void hexprint(const unsigned char *data, size_t len, bool print0x)
{
    if (print0x) {
        fprintf(stdout, "<");
    }

    for (size_t i = 0; i < len; ++i) {
        if (i % 4 == 0 && i != 0)
            fprintf(stdout, " ");
        fprintf(stdout, "%02x", data[i]);
    }

    if (print0x) {
        fprintf(stdout, ">");
    }
}

int amg_dump_record(FILE *file)
{
    ds_record_t *record = ds_record_create();
    int status = ds_record_fread(record, file);
    if (status != 0) {
        ds_record_free(record);
        return 1;
    }

    fprintf(stdout, "\n");

    CFStringRef filename = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                        ds_record_get_filename_ptr(record),
                                                        (CFIndex)ds_record_get_filename_len(record));
    fprintf(stdout, "filename: %s\n", CFStringGetCStringPtr(filename, kCFStringEncodingUTF8));
    CFRelease(filename);

    const uint32_t record_type_n = htonl(ds_record_get_type(record));
    fprintf(stdout, "record type: '%.4s' (%u)\n",
            (const char *)&record_type_n,
            (unsigned int)(ds_record_get_type(record)));

    const uint32_t data_type_n = htonl(ds_record_get_data_type(record));
    fprintf(stdout, "data type: '%.4s' (%u)\n",
            (const char *)&data_type_n,
            (unsigned int)(ds_record_get_data_type(record)));

    fprintf(stdout, "data: ");
    switch (ds_record_get_data_type(record)) {
        case ds_record_data_type_blob: {
            const uint8_t *data = ds_record_get_data_as_blob_ptr(record);
            size_t size = ds_record_get_data_as_blob_size(record);

            if (ds_record_get_type(record) == ds_record_type_BKGD) {
                if (size == 12) {
                    const uint32_t BKGDtype = uint32_from_be(data);
                    const uint32_t ClrB = FOUR_CHAR_CODE('ClrB'), PctB = FOUR_CHAR_CODE('PctB');

                    fprintf(stdout, "'%.4s' - ", data);
                    data += 4;
                    size -= 4;

                    if (BKGDtype == ClrB) {
                        fprintf(stdout, "rgb(%hu, %hu, %hu) - ",
                                uint16_from_be(data + 0),
                                uint16_from_be(data + 2),
                                uint16_from_be(data + 4));
                        data += 6;
                        size -= 6;
                    } else if (BKGDtype == PctB) {
                        uint32_t pict_len = uint32_from_be(data);
                        fprintf(stdout, "%u bytes - ", pict_len);
                        data += 4;
                        size -= 4;
                    }
                } else {
                    fprintf(stderr, "warning: '%.4s' record is of wrong size %zu; expected 12\n", (const char *)&record_type_n, size);
                }

                hexprint(data, size, true);
            } else if (ds_record_get_type(record) == ds_record_type_Iloc) {
                if (size == 16) {
                    const Iloc_t iconLocation = ds_record_get_data_as_Iloc(record);
                    fprintf(stdout, "{ x = %u, y = %u, unknown = ", iconLocation.x, iconLocation.y);
                    hexprint(iconLocation.unknown, sizeof(iconLocation.unknown), true);
                    fprintf(stdout, " }");
                } else {
                    fprintf(stderr, "warning: '%.4s' record is of wrong size %zu; expected 16\n", (const char *)&record_type_n, size);
                    hexprint(data, size, true);
                }
            } else if (ds_record_get_type(record) == ds_record_type_fwi0) {
                if (size == 16) {
                    const fwi0_t windowInfo = ds_record_get_data_as_fwi0(record);
                    const uint32_t view_n = htonl(windowInfo.view);
                    fprintf(stdout, "{ top = %hu, left = %hu, bottom = %hu, right = %hu, view = '%.4s', unknown = ",
                            windowInfo.top, windowInfo.left, windowInfo.bottom, windowInfo.right, (const char *)&view_n);
                    hexprint(windowInfo.unknown, sizeof(windowInfo.unknown), true);
                    fprintf(stdout, " }");
                } else {
                    fprintf(stderr, "warning: '%.4s' record is of wrong size %zu; expected 16\n", (const char *)&record_type_n, size);
                    hexprint(data, size, true);
                }
            } else {
                const size_t plist_text_len = ds_record_get_data_as_plist_ustr_len(record);
                if (plist_text_len) {
                    CFStringRef plist_text = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                                          ds_record_get_data_as_plist_ustr_ptr(record),
                                                                          (CFIndex)plist_text_len);
                    fprintf(stdout, "%s", CFStringGetCStringPtr(plist_text, kCFStringEncodingUTF8));
                    CFRelease(plist_text);
                } else {
                    fprintf(stdout, "%zu bytes - ", size);
                    hexprint(data, size, true);
                }
            }

            break;
        }
        case ds_record_data_type_bool:
            fprintf(stdout, "%s", ds_record_get_data_as_bool(record) ? "true" : "false");
            break;
        case ds_record_data_type_comp:
            fprintf(stdout, "%llu", ds_record_get_data_as_comp(record));
            break;
        case ds_record_data_type_dutc: {
            UTCDateTime dutc = ds_record_get_data_as_dutc(record);
            fprintf(stdout, "%hu/%u/%hu",
                    dutc.highSeconds,
                    (unsigned int)dutc.lowSeconds,
                    dutc.fraction);
            break;
        }
        case ds_record_data_type_long:
            fprintf(stdout, "%u", ds_record_get_data_as_long(record));
            break;
        case ds_record_data_type_shor:
            fprintf(stdout, "%hu", ds_record_get_data_as_shor(record));
            break;
        case ds_record_data_type_type:
            fprintf(stdout, "%u", (unsigned int)ds_record_get_data_as_type(record));
            break;
        case ds_record_data_type_ustr: {
            CFStringRef recordfilename = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                                      ds_record_get_data_as_ustr_ptr(record),
                                                                      (CFIndex)ds_record_get_data_as_ustr_len(record));
            fprintf(stdout, "%s", CFStringGetCStringPtr(recordfilename, kCFStringEncodingUTF8));
            CFRelease(recordfilename);
            break;
        }
    }
    fprintf(stdout, "\n");
    
    return 0;
}
