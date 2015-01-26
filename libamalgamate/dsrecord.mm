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

#include "dsrecord_p.h"
#import <Foundation/Foundation.h>

void _ds_record::update_plist() {
    if (data_plist) {
        CFRelease(data_plist);
    }
    CFDataRef cfdata = CFDataCreate(kCFAllocatorDefault, data_blob.data(), static_cast<CFIndex>(data_blob.size()));
    data_plist = CFPropertyListCreateWithData(kCFAllocatorDefault, cfdata, 0, NULL, NULL);
    CFRelease(cfdata);
}

void _ds_record::update_plist_ustr()
{
#if __has_feature(objc_arc)
    @autoreleasepool {
#else
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
#endif
    update_plist();
    NSString *str = [(__bridge id)data_plist description];
    if ([str length] > 0) {
        std::vector<uint16_t> bits;
        bits.resize([str length]);
        [str getCharacters:bits.data()];
        data_plist_ustr = std::basic_string<uint16_t>(bits.data(), bits.size());
    } else {
        data_plist_ustr = std::basic_string<uint16_t>();
    }
#if __has_feature(objc_arc)
    }
#else
    [pool release];
#endif
}

CFPropertyListRef ds_record_get_data_as_plist(ds_record_t *record) {
    assert(record);
    return record->data_plist;
}

void ds_record_copy_data_as_plist_ustr(ds_record_t *record, uint16_t *ustr)
{
    assert(record);
    assert(ustr);

    memcpy(ustr, record->data_plist_ustr.data(), record->data_plist_ustr.size());
}

const uint16_t *ds_record_get_data_as_plist_ustr_ptr(ds_record_t *record)
{
    assert(record);
    return record->data_plist_ustr.data();
}

size_t ds_record_get_data_as_plist_ustr_len(ds_record_t *record)
{
    assert(record);
    return record->data_plist_ustr.size();
}

std::basic_string<uint16_t> ds_record_get_data_as_plist_ustr(ds_record_t *record)
{
    assert(record);
    return record->data_plist_ustr;
}
