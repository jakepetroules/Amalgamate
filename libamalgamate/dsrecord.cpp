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
#include "dsrecord_p.h"
#include "amgmemory.h"

_ds_record::_ds_record()
: filename(), record_type(), data_type(), data(), data_blob(), data_ustr(), data_plist(), data_plist_ustr()
{
}

_ds_record::~_ds_record() {
    if (data_plist) {
        CFRelease(data_plist);
    }
}

ds_record_t *ds_record_create(void)
{
    return new _ds_record();
}

void ds_record_free(ds_record_t *record)
{
    delete record;
}

CFDictionaryRef ds_record_copy_dictionary(ds_record_t *record)
{
    CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                            &kCFTypeDictionaryKeyCallBacks,
                                                            &kCFTypeDictionaryValueCallBacks);
    AMCFTypeRef<CFStringRef> str(CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                              ds_record_get_filename_ptr(record),
                                                              static_cast<CFIndex>(ds_record_get_filename_len(record))));
    CFDictionarySetValue(dict, CFSTR("filename"), str);

    const uint32_t record_type_n = htonl(ds_record_get_type(record));
    const char *record_type_ptr = reinterpret_cast<const char *>(&record_type_n);
    AMCFTypeRef<CFStringRef> record_type_str(CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%c%c%c%c"),
                                                                      record_type_ptr[0],
                                                                      record_type_ptr[1],
                                                                      record_type_ptr[2],
                                                                      record_type_ptr[3]));
    CFDictionarySetValue(dict, CFSTR("type"), record_type_str);

    const uint32_t data_type_n = htonl(ds_record_get_data_type(record));
    const char *data_type_ptr = reinterpret_cast<const char *>(&data_type_n);
    AMCFTypeRef<CFStringRef> data_type_str(CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%c%c%c%c"),
                                                                    data_type_ptr[0],
                                                                    data_type_ptr[1],
                                                                    data_type_ptr[2],
                                                                    data_type_ptr[3]));
    CFDictionarySetValue(dict, CFSTR("data_type"), data_type_str);

    switch (ds_record_get_data_type(record)) {
        case ds_record_data_type_long: {
            const uint32_t value = ds_record_get_data_as_long(record);
            AMCFTypeRef<CFNumberRef> number(CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &value));
            CFDictionarySetValue(dict, CFSTR("data"), number);
            break;
        }
        case ds_record_data_type_shor: {
            const uint16_t value = ds_record_get_data_as_shor(record);
            AMCFTypeRef<CFNumberRef> number(CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &value));
            CFDictionarySetValue(dict, CFSTR("data"), number);
            break;
        }
        case ds_record_data_type_bool: {
            CFDictionarySetValue(dict, CFSTR("data"),
                                 ds_record_get_data_as_bool(record)
                                 ? kCFBooleanTrue
                                 : kCFBooleanFalse);
            break;
        }
        case ds_record_data_type_blob: {
            const uint8_t *data = ds_record_get_data_as_blob_ptr(record);
            size_t size = ds_record_get_data_as_blob_size(record);

            if (ds_record_get_type(record) == ds_record_type_BKGD) {
                if (size == 12) {
                    AMCFTypeRef<CFMutableDictionaryRef> bkgd(CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                                       &kCFTypeDictionaryKeyCallBacks,
                                                                                       &kCFTypeDictionaryValueCallBacks));

                    const uint32_t BKGDtype = uint32_from_be(data);
                    const uint32_t ClrB = FOUR_CHAR_CODE('ClrB'), PctB = FOUR_CHAR_CODE('PctB');

                    data += 4;
                    size -= 4;

                    if (BKGDtype == ClrB) {
                        const uint16_t r = uint16_from_be(data + 0);
                        const uint16_t g = uint16_from_be(data + 2);
                        const uint16_t b = uint16_from_be(data + 4);

                        CFDictionarySetValue(bkgd, CFSTR("r"),
                                             AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                     kCFNumberShortType,
                                                                                     &r)));
                        CFDictionarySetValue(bkgd, CFSTR("g"),
                                             AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                     kCFNumberShortType,
                                                                                     &g)));
                        CFDictionarySetValue(bkgd, CFSTR("b"),
                                             AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                     kCFNumberShortType,
                                                                                     &b)));

                        data += 6;
                        size -= 6;
                    } else if (BKGDtype == PctB) {
                        const uint32_t pict_len = uint32_from_be(data);
                        data += 4;
                        size -= 4;

                        CFDictionarySetValue(bkgd, CFSTR("pict"),
                                             AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                     kCFNumberIntType,
                                                                                     &pict_len)));
                    }

                    CFDictionarySetValue(dict, CFSTR("data"), bkgd);
                } else {
                    fprintf(stderr, "warning: '%.4s' record is of wrong size %zu; expected 12\n", reinterpret_cast<const char *>(&record_type_n), size);
                }
            } else if (ds_record_get_type(record) == ds_record_type_Iloc) {
                if (size == 16) {
                    const Iloc_t iconLocation = ds_record_get_data_as_Iloc(record);
                    AMCFTypeRef<CFMutableDictionaryRef> iloc(CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                                       &kCFTypeDictionaryKeyCallBacks,
                                                                                       &kCFTypeDictionaryValueCallBacks));
                    CFDictionarySetValue(iloc, CFSTR("x"),
                                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                 kCFNumberIntType,
                                                                                 &iconLocation.x)));
                    CFDictionarySetValue(iloc, CFSTR("y"),
                                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                 kCFNumberIntType,
                                                                                 &iconLocation.y)));
                    CFDictionarySetValue(iloc, CFSTR("unknown"),
                                         AMCFTypeRef<CFDataRef>(CFDataCreate(kCFAllocatorDefault,
                                                                             iconLocation.unknown,
                                                                             sizeof(iconLocation.unknown))));
                    CFDictionarySetValue(dict, CFSTR("data"), iloc);
                } else {
                    fprintf(stderr, "warning: '%.4s' record is of wrong size %zu; expected 16\n", reinterpret_cast<const char *>(&record_type_n), size);
                }
            } else if (ds_record_get_type(record) == ds_record_type_fwi0) {
                if (size == 16) {
                    const fwi0_t windowInfo = ds_record_get_data_as_fwi0(record);
                    const uint32_t view_n = htonl(windowInfo.view);
                    const char *view_ptr = reinterpret_cast<const char *>(&view_n);

                    AMCFTypeRef<CFMutableDictionaryRef> fwi0(CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                                       &kCFTypeDictionaryKeyCallBacks,
                                                                                       &kCFTypeDictionaryValueCallBacks));

                    CFDictionarySetValue(fwi0, CFSTR("top"),
                                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                 kCFNumberShortType,
                                                                                 &windowInfo.top)));
                    CFDictionarySetValue(fwi0, CFSTR("left"),
                                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                 kCFNumberShortType,
                                                                                 &windowInfo.left)));
                    CFDictionarySetValue(fwi0, CFSTR("bottom"),
                                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                 kCFNumberShortType,
                                                                                 &windowInfo.bottom)));
                    CFDictionarySetValue(fwi0, CFSTR("right"),
                                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                                 kCFNumberShortType,
                                                                                 &windowInfo.right)));
                    CFDictionarySetValue(fwi0, CFSTR("view"), AMCFTypeRef<CFStringRef>(CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%c%c%c%c"),
                                                                                                                view_ptr[0],
                                                                                                                view_ptr[1],
                                                                                                                view_ptr[2],
                                                                                                                view_ptr[3])));
                    CFDictionarySetValue(fwi0, CFSTR("unknown"),
                                         AMCFTypeRef<CFDataRef>(CFDataCreate(kCFAllocatorDefault,
                                                                             windowInfo.unknown,
                                                                             sizeof(windowInfo.unknown))));
                    CFDictionarySetValue(dict, CFSTR("data"), fwi0);
                } else {
                    fprintf(stderr, "warning: '%.4s' record is of wrong size %zu; expected 16\n", reinterpret_cast<const char *>(&record_type_n), size);
                }
            } else {
                CFPropertyListRef plist = ds_record_get_data_as_plist(record);
                if (plist) {
                    CFDictionarySetValue(dict, CFSTR("data"), plist);
                } else {
                    AMCFTypeRef<CFDataRef> cfdata(CFDataCreate(kCFAllocatorDefault,
                                                               ds_record_get_data_as_blob_ptr(record),
                                                               static_cast<CFIndex>(ds_record_get_data_as_blob_size(record))));
                    CFDictionarySetValue(dict, CFSTR("data"), cfdata);
                }
            }
            break;
        }
        case ds_record_data_type_type: {
            const uint32_t type = htonl(ds_record_get_data_as_type(record));
            const char *type_ptr = reinterpret_cast<const char *>(&type);
            AMCFTypeRef<CFStringRef> type_str(CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%c%c%c%c"),
                                                                       type_ptr[0],
                                                                       type_ptr[1],
                                                                       type_ptr[2],
                                                                       type_ptr[3]));
            CFDictionarySetValue(dict, CFSTR("data"), type_str);
            break;
        }
        case ds_record_data_type_ustr: {
            AMCFTypeRef<CFStringRef> string(CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                                         ds_record_get_data_as_ustr_ptr(record),
                                                                         static_cast<CFIndex>(ds_record_get_data_as_ustr_len(record))));
            CFDictionarySetValue(dict, CFSTR("data"), string);
            break;
        }
        case ds_record_data_type_comp: {
            const uint64_t value = ds_record_get_data_as_comp(record);
            AMCFTypeRef<CFNumberRef> number(CFNumberCreate(kCFAllocatorDefault, kCFNumberLongLongType, &value));
            CFDictionarySetValue(dict, CFSTR("data"), number);
            break;
        }
        case ds_record_data_type_dutc: {
            const UTCDateTime value = ds_record_get_data_as_dutc(record);
            CFAbsoluteTime cfvalue = DBL_MIN;
            if (UCConvertUTCDateTimeToCFAbsoluteTime(&value, &cfvalue) != noErr) {
                fprintf(stderr, "error converting UTCDateTime to CFAbsoluteTime");
                return nullptr;
            }
            AMCFTypeRef<CFDateRef> date(CFDateCreate(kCFAllocatorDefault, cfvalue));
            CFDictionarySetValue(dict, CFSTR("data"), date);
            break;
        }
    }

    return dict;
}

size_t ds_record_get_filename_len(ds_record_t *record)
{
    assert(record);
    return record->filename.size();
}

void ds_record_copy_filename(ds_record_t *record, uint16_t *ustr)
{
    assert(record);
    assert(ustr);

    memcpy(ustr, record->filename.data(), record->filename.size());
}

const uint16_t *ds_record_get_filename_ptr(ds_record_t *record)
{
    assert(record);
    return record->filename.data();
}

ds_record_type ds_record_get_type(ds_record_t *record)
{
    assert(record);
    return record->record_type;
}

ds_record_data_type ds_record_get_data_type(ds_record_t *record)
{
    assert(record);
    return record->data_type;
}

uint32_t ds_record_get_data_as_long(ds_record_t *record)
{
    assert(record);
    return record->data.llong;
}

uint16_t ds_record_get_data_as_shor(ds_record_t *record)
{
    assert(record);
    return record->data.shor;
}

bool ds_record_get_data_as_bool(ds_record_t *record)
{
    assert(record);
    return record->data.bbool;
}

void ds_record_copy_data_as_blob(ds_record_t *record, unsigned char *blob)
{
    assert(record);
    assert(blob);

    memcpy(blob, record->data_blob.data(), record->data_blob.size());
}

const unsigned char *ds_record_get_data_as_blob_ptr(ds_record_t *record)
{
    assert(record);
    return record->data_blob.data();
}

size_t ds_record_get_data_as_blob_size(ds_record_t *record)
{
    assert(record);
    return record->data_blob.size();
}

FourCharCode ds_record_get_data_as_type(ds_record_t *record)
{
    assert(record);
    return record->data.type;
}

void ds_copy_record_data_as_ustr(ds_record_t *record, uint16_t *ustr)
{
    assert(record);
    assert(ustr);

    memcpy(ustr, record->data_ustr.data(), record->data_ustr.size());
}

const uint16_t *ds_record_get_data_as_ustr_ptr(ds_record_t *record)
{
    assert(record);
    return record->data_ustr.data();
}

size_t ds_record_get_data_as_ustr_len(ds_record_t *record)
{
    assert(record);
    return record->data_ustr.size();
}

Iloc_t ds_record_get_data_as_Iloc(ds_record_t *record)
{
    assert(record);
    assert(ds_record_get_data_as_blob_size(record) == 16);
    const unsigned char *data = ds_record_get_data_as_blob_ptr(record);
    Iloc_t iconLocation;
    iconLocation.x = ntohl(*reinterpret_cast<const uint32_t *>(data));
    data += sizeof(iconLocation.x);
    iconLocation.y = ntohl(*reinterpret_cast<const uint32_t *>(data));
    data += sizeof(iconLocation.y);
    memcpy(iconLocation.unknown, data, sizeof(iconLocation.unknown));
    return iconLocation;
}

fwi0_t ds_record_get_data_as_fwi0(ds_record_t *record)
{
    assert(record);
    assert(ds_record_get_data_as_blob_size(record) == 16);
    const unsigned char *data = ds_record_get_data_as_blob_ptr(record);
    fwi0_t windowInfo;
    windowInfo.top = ntohs(*reinterpret_cast<const uint16_t *>(data));
    data += sizeof(windowInfo.top);
    windowInfo.left = ntohs(*reinterpret_cast<const uint16_t *>(data));
    data += sizeof(windowInfo.left);
    windowInfo.bottom = ntohs(*reinterpret_cast<const uint16_t *>(data));
    data += sizeof(windowInfo.bottom);
    windowInfo.right = ntohs(*reinterpret_cast<const uint16_t *>(data));
    data += sizeof(windowInfo.right);
    windowInfo.view = ntohl(*reinterpret_cast<const uint32_t *>(data));
    data += sizeof(windowInfo.view);
    memcpy(windowInfo.unknown, data, sizeof(windowInfo.unknown));
    return windowInfo;
}

uint64_t ds_record_get_data_as_comp(ds_record_t *record)
{
    assert(record);
    return record->data.comp;
}

UTCDateTime ds_record_get_data_as_dutc(ds_record_t *record)
{
    assert(record);
    return record->data.dutc;
}

std::basic_string<uint16_t> ds_record_get_filename(ds_record_t *record)
{
    assert(record);
    return record->filename;
}

std::vector<unsigned char> ds_record_get_data_as_blob(ds_record_t *record)
{
    assert(record);
    return record->data_blob;
}

std::basic_string<uint16_t> ds_record_get_data_as_ustr(ds_record_t *record)
{
    assert(record);
    return record->data_ustr;
}

void ds_record_set_filename(ds_record_t *record, const uint16_t *ustr, size_t len)
{
    assert(record);
    record->filename = std::basic_string<uint16_t>(ustr, len);
}

void ds_record_set_type(ds_record_t *record, ds_record_type type)
{
    assert(record);
    record->record_type = type;
}

void ds_record_set_data_type(ds_record_t *record, ds_record_data_type type)
{
    assert(record);
    record->data_type = type;
}

void ds_record_set_data_as_long(ds_record_t *record, uint32_t llong)
{
    assert(record);
    record->data.llong = llong;
}

void ds_record_set_data_as_shor(ds_record_t *record, uint16_t shor)
{
    assert(record);
    record->data.shor = shor;
}

void ds_record_set_data_as_bool(ds_record_t *record, bool bbool)
{
    assert(record);
    record->data.bbool = bbool;
}

void ds_record_set_data_as_blob(ds_record_t *record, const unsigned char *blob, size_t len)
{
    assert(record);
    record->data_blob = std::vector<unsigned char>(len);
    memcpy(record->data_blob.data(), blob, len);
    record->update_plist_ustr();
}

void ds_record_set_data_as_type(ds_record_t *record, FourCharCode type)
{
    assert(record);
    record->data.type = type;
}

void ds_record_set_data_as_ustr(ds_record_t *record, const uint16_t *ustr, size_t len)
{
    assert(record);
    record->data_ustr = std::basic_string<uint16_t>(ustr, len);
}

void ds_record_set_data_as_comp(ds_record_t *record, uint64_t comp)
{
    assert(record);
    record->data.comp = comp;
}

void ds_record_set_data_as_dutc(ds_record_t *record, UTCDateTime dutc)
{
    assert(record);
    record->data.dutc = dutc;
}

void ds_record_set_filename_obj(ds_record_t *record, const std::basic_string<uint16_t> &filename)
{
    assert(record);
    record->filename = filename;
}

void ds_record_set_data_as_blob_obj(ds_record_t *record, const std::vector<unsigned char> &blob)
{
    assert(record);
    record->data_blob = blob;
}

void ds_record_set_data_as_ustr_obj(ds_record_t *record, const std::basic_string<uint16_t> &ustr)
{
    assert(record);
    record->data_ustr = ustr;
}

int ds_record_fread(ds_record_t *record, FILE *file)
{
    assert(record);
    assert(file);

    uint32_t filename_length;
    if (fread_uint32_be(&filename_length, file) != 1)
    {
        fprintf(stderr, "error reading record filename length\n");
        return 1;
    }

    if (filename_length == 0)
    {
        fprintf(stderr, "unexpected record filename length %u\n", filename_length);
        return 1;
    }

    size_t n;
    std::vector<uint16_t> chars(filename_length);
    if ((n = fread(chars.data(), sizeof(chars[0]), chars.size(), file)) != chars.size())
    {
        fprintf(stderr, "error reading record filename (expected %zu characters, got %zu)\n", chars.size(), n);
        return 1;
    }

    for (size_t i = 0; i < chars.size(); ++i)
        chars[i] = ntohs(chars[i]);

    record->filename = std::basic_string<uint16_t>(chars.data(), chars.size());

    uint32_t record_type;
    if (fread_uint32_be(&record_type, file) != 1)
    {
        fprintf(stderr, "error reading record type\n");
        return 1;
    }

    record->record_type = static_cast<ds_record_type>(record_type);

    uint32_t data_type;
    if (fread_uint32_be(&data_type, file) != 1)
    {
        fprintf(stderr, "error reading record data type\n");
        return 1;
    }

    record->data_type = static_cast<ds_record_data_type>(data_type);

    // If we encounter a seemingly incorrect data type for a particular record
    // type we'll just warn instead of failing because technically we can still
    // read the record if the data type is known to us
    uint32_t data_type_n = htonl(record->data_type);
    uint32_t record_type_n = htonl(record->record_type);
    ds_record_data_type expected_data_type = ds_record_data_type_for_record_type(record->record_type);
    uint32_t expected_data_type_n = htonl(static_cast<uint32_t>(expected_data_type));
    if (expected_data_type == 0) {
        fprintf(stderr, "warning: unknown record type '%.4s'\n",
                reinterpret_cast<const char *>(&record_type_n));
    } else if (expected_data_type != record->data_type) {
        fprintf(stderr, "warning: unexpected data type '%.4s' for record type '%.4s'; expected '%.4s'\n",
                reinterpret_cast<const char *>(&data_type_n),
                reinterpret_cast<const char *>(&record_type_n),
                reinterpret_cast<const char *>(&expected_data_type_n));
    }

    uint32_t expected = 1; // expected elements (not bytes)
    switch (record->data_type)
    {
        case ds_record_data_type_bool:
            n = fread(&record->data.bbool, sizeof(record->data.bbool), 1, file);
            break;
        case ds_record_data_type_comp:
            n = fread_uint64_be(&record->data.comp, file);
            break;
        case ds_record_data_type_dutc:
            n = fread_uint64_be(reinterpret_cast<uint64_t *>(&record->data.dutc), file);
            break;
        case ds_record_data_type_long:
            n = fread_uint32_be(&record->data.llong, file);
            break;
        case ds_record_data_type_shor:
        {
            uint8_t c[2];
            if (fread(c, sizeof(c[0]), 2, file) != 2)
            {
                fprintf(stderr, "error reading record data\n");
                return 1;
            }

            // 'shor' is a 16-bit integer but physically stored as 32-bits for some reason
            if (c[0] != 0 || c[1] != 0)
                fprintf(stderr, "warning: nonzero leading bytes in record data type '%.4s'\n",
                        reinterpret_cast<const char *>(&data_type_n));

            n = fread_uint16_be(&record->data.shor, file);
            break;
        }
        case ds_record_data_type_type:
            n = fread_uint32_be(reinterpret_cast<uint32_t *>(&record->data.type), file);
            break;
        case ds_record_data_type_blob:
        case ds_record_data_type_ustr:
        {
            if ((fread_uint32_be(&expected, file)) != 1) {
                fprintf(stderr, "error reading record data size\n");
                return 1;
            }

            if (expected == 0) {
                fprintf(stderr, "unexpected record data length %u\n", expected);
                return 1;
            }

            if (record->data_type == ds_record_data_type_blob) {
                record->data_blob.resize(expected);
                n = fread(record->data_blob.data(), sizeof(record->data_blob[0]), record->data_blob.size(), file);
                record->update_plist_ustr();
            }

            if (record->data_type == ds_record_data_type_ustr) {
                std::vector<uint16_t> ustr(expected);
                n = fread(ustr.data(), sizeof(ustr[0]), ustr.size(), file);
                for (size_t i = 0; i < ustr.size(); ++i)
                    ustr[i] = ntohs(ustr[i]);
                record->data_ustr = std::basic_string<uint16_t>(ustr.data(), ustr.size());
            }

            break;
        }
        default:
            // We must fail on unknown data types because we cannot know their size
            fprintf(stderr, "unknown record data type '%.4s'\n",
                    reinterpret_cast<const char *>(&data_type_n));
            return 1;
    }

    if (n != expected) {
        fprintf(stderr, "wrong number of record data elements %zu, expected %u\n", n, expected);
        return 1;
    }

    return 0;
}

ds_record_data_type ds_record_data_type_for_record_type(ds_record_type record_type)
{
    switch (record_type)
    {
        case ds_record_type_BKGD:
            return ds_record_data_type_blob;
        case ds_record_type_ICVO:
            return ds_record_data_type_bool;
        case ds_record_type_Iloc:
            return ds_record_data_type_blob;
        case ds_record_type_LSVO:
            return ds_record_data_type_bool;
        case ds_record_type_bwsp:
            return ds_record_data_type_blob;
        case ds_record_type_cmmt:
            return ds_record_data_type_ustr;
        case ds_record_type_dilc:
            return ds_record_data_type_blob;
        case ds_record_type_dscl:
            return ds_record_data_type_bool;
        case ds_record_type_extn:
            return ds_record_data_type_ustr;
        case ds_record_type_fwi0:
            return ds_record_data_type_blob;
        case ds_record_type_fwsw:
            return ds_record_data_type_long;
        case ds_record_type_fwvh:
            return ds_record_data_type_shor;
        case ds_record_type_GRP0:
            return ds_record_data_type_ustr;
        case ds_record_type_icgo:
            return ds_record_data_type_blob;
        case ds_record_type_icsp:
            return ds_record_data_type_blob;
        case ds_record_type_icvo:
            return ds_record_data_type_blob;
        case ds_record_type_icvp:
            return ds_record_data_type_blob;
        case ds_record_type_icvP:
            return ds_record_data_type_blob;
        case ds_record_type_icvt:
            return ds_record_data_type_shor;
        case ds_record_type_info:
            return ds_record_data_type_blob;
        case ds_record_type_logS:
            return ds_record_data_type_comp;
        case ds_record_type_lg1S:
            return ds_record_data_type_comp;
        case ds_record_type_lssp:
            return ds_record_data_type_blob;
        case ds_record_type_lsvo:
            return ds_record_data_type_blob;
        case ds_record_type_lsvt:
            return ds_record_data_type_shor;
        case ds_record_type_lsvp:
            return ds_record_data_type_blob;
        case ds_record_type_lsvP:
            return ds_record_data_type_blob;
        case ds_record_type_modD:
            return ds_record_data_type_dutc;
        case ds_record_type_moDD:
            return ds_record_data_type_dutc;
        case ds_record_type_phyS:
            return ds_record_data_type_comp;
        case ds_record_type_ph1S:
            return ds_record_data_type_comp;
        case ds_record_type_pict:
            return ds_record_data_type_blob;
        case ds_record_type_vSrn:
            return ds_record_data_type_long;
        case ds_record_type_vstl:
            return ds_record_data_type_type;
    }
    
    return static_cast<ds_record_data_type>(0);
}
