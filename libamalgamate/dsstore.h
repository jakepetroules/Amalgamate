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

#ifndef PETROULES_AMALGAMATE_DSSTORE_H
#define PETROULES_AMALGAMATE_DSSTORE_H

// .DS_Store
// https://github.com/dscho/dsstore
// http://search.cpan.org/~wiml/Mac-Finder-DSStore-1.00/DSStore.pm
// http://search.cpan.org/~wiml/Mac-Finder-DSStore-1.00/DSStoreFormat.pod
// https://github.com/DanLipsitt/ds_store // Python
// https://github.com/phooky/ds_store // Python
// https://github.com/tommetge/dsstore // Objective-C
// https://github.com/LinusU/node-ds-store // JavaScript (node.js)
// https://bitbucket.org/fourplusone/dsstore-xml/src // Perl

// Macintosh Aliases
// http://sebastien.kirche.free.fr/python_stuff/MacOS-aliases.txt
// http://cpansearch.perl.org/src/WIML/Mac-Alias-Parse-0.20/Parse.pm

#include "amgexport.h"
#include <stddef.h>
#import <Carbon/Carbon.h>

const FourCharCode kDSHeaderMagic = FOUR_CHAR_CODE('Bud1');

typedef struct {
    uint32_t version;
    FourCharCode magic;

    uint32_t allocator_offset; // "rootAddress"
    uint32_t allocator_size;
    uint32_t allocator_offset2; // "checkAddr"
    uint8_t padding[16];
} dsstore_header_t;

void dsstore_header_init(dsstore_header_t *header);

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

typedef struct {
    uint32_t root_block_number;
    uint32_t node_levels;
    uint32_t record_count;
    uint32_t node_count;
    uint32_t tree_node_page_size; // ??? always 0x1000
} dsstore_header_block_t;

#endif
