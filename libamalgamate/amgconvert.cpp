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

#include "amgconvert.h"
#include "amgmemory.h"
#include "dsio.h"
#include "dsrecord.h"

typedef std::shared_ptr<std::FILE> shared_file_ptr;
static shared_file_ptr make_shared_file(const char *filename, const char *flags) {
    std::FILE *const fp = std::fopen(filename, flags);
    return fp ? shared_file_ptr(fp, std::fclose) : shared_file_ptr();
}

int amg_convert_file(const char *filename, const char *format) {
    assert(filename);

    int ret = 0;
    shared_file_ptr file = nullptr;

    if (strcmp(format, "json") != 0) {
        fprintf(stderr, "error: format '%s' is not supported\n", format);
        return 1;
    }

    file = make_shared_file(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "error opening file %s\n", filename);
        return 1;
    }

    ds_store_t *store = ds_store_fread(file.get());
    if (!store) {
        return 1;
    }

    AMCFTypeRef<CFMutableArrayRef> records(CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks));

    if (ds_store_enum_records_core(store, [&records](ds_record_t *record) {
        CFArrayAppendValue(records, AMCFTypeRef<CFDictionaryRef>(ds_record_copy_dictionary(record)));
    }) != 0) {
        fprintf(stderr, "error enumerating records\n");
        return 1;
    }

    AMCFTypeRef<CFDataRef> data(CFPropertyListCreateData(kCFAllocatorDefault, records, kCFPropertyListXMLFormat_v1_0, 0, NULL));
    fprintf(stdout, "%.*s\n", static_cast<int>(CFDataGetLength(data)), CFDataGetBytePtr(data));

    return ret;
}

