//
//  dsstore.h
//  Amalgamate
//
//  Created by Jake Petroules on 2014-05-18.
//
//

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
