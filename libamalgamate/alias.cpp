/*
 * Copyright (c) 2017 Jake Petroules. All rights reserved.
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

#include "alias.h"
#include "dsio.h"
#include <cassert>
#include <vector>

typedef struct {
    uint32_t creator_code;
    uint16_t record_size;
    uint16_t version;
    uint16_t alias_kind;
    struct {
        uint8_t length;
        char data[27];
        char padding[2]; // unused
    } volume_name;
    uint32_t volume_date;
    uint16_t filesystem_type;
    uint16_t disk_type;
    uint32_t containing_folder_cnid;
    struct {
        uint8_t length;
        char data[63];
    } target_name;
    uint32_t target_cnid;
    uint32_t target_creation_date;
    uint32_t target_creator_code;
    uint32_t target_type_code;
    int16_t alias_to_root_directory_depth;
    int16_t root_to_target_directory_depth;
    uint32_t volume_attributes;
    uint16_t volume_fsid;
    unsigned char reserved[10]; // all zeros
} alias_header_t;

static const size_t alias_header_size = 150;

typedef struct {
    int16_t tag;
    uint16_t length;
    unsigned char value[65536];
} metadata_entry_t;

struct _alias {
    _alias();
    alias_header_t header;
    std::vector<metadata_entry_t> metadata_entries;

private:
    _alias(const _alias &) = delete;
    _alias &operator=(const _alias &) = delete;
};

_alias::_alias() {
    memset(&header, 0, sizeof(header));
    header.record_size = alias_header_size;
    header.version = 2;
}

alias_t *alias_create(void)
{
    return new _alias();
}

alias_t *alias_create_from_data(const unsigned char *data, size_t size)
{
    assert(data);
    assert(size >= alias_header_size);
    const unsigned char * const data_start = data;

    alias_t *alias = alias_create();

    data = read_uint32_from_be(data, &alias->header.creator_code);
    data = read_uint16_from_be(data, &alias->header.record_size);
    if (alias->header.record_size != size) {
        fprintf(stderr, "unexpected header size: %hu, expected: %zu",
                alias->header.record_size, size);
        alias_free(alias);
        return nullptr;
    }

    data = read_uint16_from_be(data, &alias->header.version);
    if (alias->header.version != 2) {
        fprintf(stderr, "unsupported alias version: %hu\n", alias->header.version);
        alias_free(alias);
        return nullptr;
    }

    data = read_uint16_from_be(data, &alias->header.alias_kind);

    data = read_uint8_from(data, &alias->header.volume_name.length);
    data = read_data_from(data,
                          reinterpret_cast<unsigned char *>(alias->header.volume_name.data),
                          sizeof(alias->header.volume_name.data));

    data = read_uint32_from_be(data, &alias->header.volume_date);
    data = read_uint16_from_be(data, &alias->header.filesystem_type);
    data = read_uint16_from_be(data, &alias->header.disk_type);
    data = read_uint32_from_be(data, &alias->header.containing_folder_cnid);


    data = read_uint8_from(data, &alias->header.target_name.length);
    data = read_data_from(data,
                          reinterpret_cast<unsigned char *>(alias->header.target_name.data),
                          sizeof(alias->header.target_name.data));

    data = read_uint32_from_be(data, &alias->header.target_cnid);
    data = read_uint32_from_be(data, &alias->header.target_creation_date);
    data = read_uint32_from_be(data, &alias->header.target_creator_code);
    data = read_uint32_from_be(data, &alias->header.target_type_code);
    data = read_int16_from_be(data, &alias->header.alias_to_root_directory_depth);
    data = read_int16_from_be(data, &alias->header.root_to_target_directory_depth);
    data = read_uint32_from_be(data, &alias->header.volume_attributes);
    data = read_uint16_from_be(data, &alias->header.volume_fsid);
    data = read_data_from(data, alias->header.reserved, sizeof(alias->header.reserved));

    while ((data - data_start) + 4 <= alias->header.record_size) {
        metadata_entry_t entry;
        data = read_int16_from_be(data, &entry.tag);
        data = read_uint16_from_be(data, &entry.length);
        data = read_data_from(data, entry.value, entry.length % 2 == 0 ? entry.length : entry.length + 1);

        if (entry.tag == -1) {
            // this record should be the last one (and not be counted)
            if ((data - data_start) != alias->header.record_size) {
                fprintf(stderr, "warning: %zd bytes of garbage data following alias record\n",
                        (data - data_start) - alias->header.record_size);
            }
            continue;
        }

        alias->metadata_entries.push_back(entry);
    }
    
    return alias;
}

void alias_free(alias_t *alias)
{
    delete alias;
}

#ifdef __APPLE__
#include "cfutils.h"
#include <CoreFoundation/CoreFoundation.h>

CFDictionaryRef _alias_copy_dictionary(const alias_t *pictRecord)
{
    CFMutableDictionaryRef pict(CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                          &kCFTypeDictionaryKeyCallBacks,
                                                          &kCFTypeDictionaryValueCallBacks));

    AMGCFDictionarySetFourCCValue(pict, CFSTR("creator_code"), pictRecord->header.creator_code);
    AMGCFDictionarySetShortValue(pict, CFSTR("record_size"), pictRecord->header.record_size);
    AMGCFDictionarySetShortValue(pict, CFSTR("version"), pictRecord->header.version);
    AMGCFDictionarySetShortValue(pict, CFSTR("alias_kind"), pictRecord->header.alias_kind);
    AMGCFDictionarySetCStringValue(pict, CFSTR("volume_name"),
                                   pictRecord->header.volume_name.data,
                                   pictRecord->header.volume_name.length);
    AMGCFDictionarySetMacDateValue(pict, CFSTR("volume_date"), pictRecord->header.volume_date);

    const uint16_t filesystem_type_n = htons(pictRecord->header.filesystem_type);
    AMGCFDictionarySetCStringValue(pict, CFSTR("filesystem_type"), reinterpret_cast<const char *>(&filesystem_type_n), sizeof(filesystem_type_n));

    AMGCFDictionarySetShortValue(pict, CFSTR("disk_type"), pictRecord->header.disk_type);
    AMGCFDictionarySetIntValue(pict, CFSTR("containing_folder_cnid"), pictRecord->header.containing_folder_cnid);
    AMGCFDictionarySetCStringValue(pict, CFSTR("target_name"), reinterpret_cast<const char *>(pictRecord->header.target_name.data), pictRecord->header.target_name.length);
    AMGCFDictionarySetIntValue(pict, CFSTR("target_cnid"), pictRecord->header.target_cnid);
    AMGCFDictionarySetMacDateValue(pict, CFSTR("target_creation_date"), pictRecord->header.target_creation_date);
    AMGCFDictionarySetFourCCValue(pict, CFSTR("target_creator_code"), pictRecord->header.target_creator_code);
    AMGCFDictionarySetIntValue(pict, CFSTR("target_type_code"), pictRecord->header.target_type_code);
    AMGCFDictionarySetSInt16Value(pict, CFSTR("alias_to_root_directory_depth"), pictRecord->header.alias_to_root_directory_depth);
    AMGCFDictionarySetSInt16Value(pict, CFSTR("root_to_target_directory_depth"), pictRecord->header.root_to_target_directory_depth);
    AMGCFDictionarySetIntValue(pict, CFSTR("volume_attributes"), pictRecord->header.volume_attributes);
    AMGCFDictionarySetShortValue(pict, CFSTR("volume_fsid"), pictRecord->header.volume_fsid);

    CFMutableArrayRef metadataItems = CFArrayCreateMutable(kCFAllocatorDefault,
                                                           static_cast<CFIndex>(pictRecord->metadata_entries.size()),
                                                           &kCFTypeArrayCallBacks);

    for (const auto &entry : pictRecord->metadata_entries) {
        AMCFTypeRef<CFMutableDictionaryRef> metadata(CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                               &kCFTypeDictionaryKeyCallBacks,
                                                                               &kCFTypeDictionaryValueCallBacks));
        AMGCFDictionarySetSInt32Value(metadata, CFSTR("tag"), entry.tag);

        switch (entry.tag) {
            case 0:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 18:
            case 19:
                AMGCFDictionarySetCStringValue(metadata, CFSTR("value"), reinterpret_cast<const char *>(entry.value), static_cast<size_t>(entry.length));
                break;
            case 14:
            case 15:
                // offset by 2 bytes (string length)
                AMGCFDictionarySetHFSStringValue(metadata, CFSTR("value"), reinterpret_cast<const char *>(entry.value + 2), static_cast<size_t>(entry.length));
                break;
            case 16:
            case 17:
                AMGCFDictionarySetUTCDateValue(metadata, CFSTR("value"), reinterpret_cast<const UTCDateTime *>(entry.value));
                break;
            case 20: {
                const alias_t *recursiveAlias = alias_create_from_data(entry.value, entry.length);
                CFDictionarySetValue(metadata, CFSTR("value"), AMCFTypeRef<CFDictionaryRef>(_alias_copy_dictionary(recursiveAlias)));
                break;
            }
            case 1: {
                if (entry.length % sizeof(uint32_t) == 0) {
                    CFMutableArrayRef cnids = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
                    for (size_t i = 0; i < entry.length / sizeof(uint32_t); ++i) {
                        uint32_t value_n = ntohl(*reinterpret_cast<const uint32_t *>(entry.value + (i * sizeof(uint32_t))));
                        CFArrayAppendValue(cnids, AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                          kCFNumberIntType,
                                                                                          &value_n)));
                    }

                    CFDictionarySetValue(metadata, CFSTR("value"), cnids);
                    break;
                } else {
                    [[clang::fallthrough]];
                }
            }
            case 9:
            case 10:
            case 21:
            default:
                CFDictionarySetValue(metadata, CFSTR("value"), AMCFTypeRef<CFDataRef>(CFDataCreate(kCFAllocatorDefault, entry.value, entry.length)));
                break;
        }

        CFArrayAppendValue(metadataItems, metadata);
    }

    CFDictionarySetValue(pict, CFSTR("metadata"), metadataItems);
    
    return pict;
}
#endif
