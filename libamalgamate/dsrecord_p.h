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

#ifndef AMALGAMATE_DSRECORD_P_H
#define AMALGAMATE_DSRECORD_P_H

#include "dsrecord.h"
#include <string>
#include <vector>

struct _ds_record {
    _ds_record();
    ~_ds_record();

    /*!
     * Filename, in big-endian UTF-16.
     */
    std::basic_string<uint16_t> filename;
    ds_record_type record_type;
    ds_record_data_type data_type;

    union {
        uint32_t llong;
        uint16_t shor;
        bool bbool;
        FourCharCode type;
        uint64_t comp;
        UTCDateTime dutc;
    } data;

    std::vector<unsigned char> data_blob;
    std::basic_string<uint16_t> data_ustr;
    CFPropertyListRef data_plist;
    std::basic_string<uint16_t> data_plist_ustr;

    void update_plist();
    void update_plist_ustr();
};

#endif // AMALGAMATE_DSRECORD_P_H
