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

_ds_record::_ds_record()
: filename(), record_type(), data_type(), data(), data_blob(), data_ustr()
{
}

ds_record_t *ds_record_create(void)
{
    return new _ds_record();
}

void ds_record_free(ds_record_t *record)
{
    delete record;
}

size_t ds_record_get_filename_len(ds_record_t *record)
{
    assert(record);
    return record->filename.size();
}

void ds_copy_record_filename(ds_record_t *record, uint16_t *ustr)
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

void ds_copy_record_data_as_blob(ds_record_t *record, unsigned char *blob)
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

size_t ds_record_get_data_as_ustr_len(ds_record_t *record)
{
    assert(record);
    return record->data_ustr.size();
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

    record->record_type = (ds_record_type)record_type;

    uint32_t data_type;
    if (fread_uint32_be(&data_type, file) != 1)
    {
        fprintf(stderr, "error reading record data type\n");
        return 1;
    }

    record->data_type = (ds_record_data_type)data_type;

    // If we encounter a seemingly incorrect data type for a particular record
    // type we'll just warn instead of failing because technically we can still
    // read the record if the data type is known to us
    uint32_t data_type_n = htonl(record->data_type);
    uint32_t record_type_n = htonl(record->record_type);
    ds_record_data_type expected_data_type = ds_record_data_type_for_record_type(record->record_type);
    uint32_t expected_data_type_n = htonl((uint32_t)expected_data_type);
    if (expected_data_type == 0) {
        fprintf(stderr, "warning: unknown record type '%.4s'\n", (const char *)&record_type_n);
    } else if (expected_data_type != record->data_type) {
        fprintf(stderr, "warning: unexpected data type '%.4s' for record type '%.4s'; expected '%.4s'\n",
                (const char *)&data_type_n, (const char *)&record_type_n, (const char *)&expected_data_type_n);
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
            n = fread_uint64_be((uint64_t *)&record->data.dutc, file);
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
                        (const char *)&data_type_n);

            n = fread_uint16_be(&record->data.shor, file);
            break;
        }
        case ds_record_data_type_type:
            n = fread_uint32_be((uint32_t *)&record->data.type, file);
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
            fprintf(stderr, "unknown record data type '%.4s'\n", (const char *)&data_type_n);
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
    
    return (ds_record_data_type)0;
}
