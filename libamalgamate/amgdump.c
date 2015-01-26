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

    ds_store_t *store = NULL;
    int ret = 0;
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "error opening file %s\n", filename);
        ret = 1;
        goto cleanup;
    }

    store = ds_store_fread(file);
    if (!store) {
        ret = 1;
        goto cleanup;
    }

    ds_store_dump_header(store);
    ds_store_dump_allocator_state(store);
    dsstore_header_dumpblock(store);

    // Dump the root block...
    //ds_store_enum_blocks(&allocator_state, &header_block, header_block.root_block_number, amg_dump_record, file);
    ds_store_enum_records(store, amg_dump_record);

cleanup:
    fclose(file);
    ds_store_free(store);
    return ret;
}

static inline void CFStringPrintToFile(FILE *file, CFStringRef str) {
    const CFStringEncoding encoding = kCFStringEncodingUTF8;
    const CFIndex filename_utf8_len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str), encoding);
    char *filename_utf8 = (char *)malloc((sizeof(char) * (size_t)filename_utf8_len) + 1);
    Boolean c = CFStringGetCString(str, filename_utf8, filename_utf8_len, encoding);
    assert(c);
    fprintf(file, "%s", filename_utf8);
    free(filename_utf8);
}

void amg_dump_record(ds_record_t *record) {
    CFDictionaryRef dict = ds_record_copy_dictionary(record);
    CFStringRef description = AMGCopyRealDescription(dict);
    CFStringPrintToFile(stdout, description);
    fprintf(stdout, "\n");
    CFRelease(description);
    CFRelease(dict);
}
