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
#include <CoreServices/CoreServices.h>

#ifdef __cplusplus
#include <functional>
#endif

static const FourCharCode kDSHeaderMagic = FOUR_CHAR_CODE('Bud1');

typedef struct _ds_record ds_record_t;

typedef struct _ds_store ds_store_t;
typedef void (*ds_store_record_func_t)(ds_record_t *record);

AMG_EXPORT AMG_EXTERN ds_store_t *ds_store_fread(FILE *file);
AMG_EXPORT AMG_EXTERN ds_store_t *ds_store_create(void);
AMG_EXPORT AMG_EXTERN void ds_store_free(ds_store_t *store);
AMG_EXPORT AMG_EXTERN int ds_store_enum_records(ds_store_t *store, ds_store_record_func_t func);

#ifdef __cplusplus
AMG_EXPORT AMG_EXTERN int ds_store_enum_records_core(ds_store_t *store, const std::function<void(ds_record_t *)> &func);
#endif

AMG_EXPORT AMG_EXTERN void ds_store_dump_header(ds_store_t *store);
AMG_EXPORT AMG_EXTERN void ds_store_dump_allocator_state(ds_store_t *store);
AMG_EXPORT AMG_EXTERN void dsstore_header_dumpblock(ds_store_t *store);

#endif
