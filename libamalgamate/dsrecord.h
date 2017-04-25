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

#ifndef AMALGAMATE_DSRECORD_H
#define AMALGAMATE_DSRECORD_H

#include "amgexport.h"
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#endif

#include <CoreServices/CoreServices.h> // HACK, for FOUR_CHAR_CODE

/*!
 * A record gives attributes to the files in the containing directory of the
 * .DS_Store, or to the directory itself.
 */
typedef struct _ds_record ds_record_t;

/*!
 * Structure ID or type identifying which property of the file the record describes.
 */
typedef enum {
    ds_record_type_BKGD = FOUR_CHAR_CODE('BKGD'),
    ds_record_type_ICVO = FOUR_CHAR_CODE('ICVO'),
    ds_record_type_Iloc = FOUR_CHAR_CODE('Iloc'), // prIconSpatialLocation
    ds_record_type_LSVO = FOUR_CHAR_CODE('LSVO'),
    ds_record_type_bwsp = FOUR_CHAR_CODE('bwsp'),
    ds_record_type_cmmt = FOUR_CHAR_CODE('cmmt'), // prFinderComment
    ds_record_type_dilc = FOUR_CHAR_CODE('dilc'), // prDesktopIconSpatialLocation
    ds_record_type_dscl = FOUR_CHAR_CODE('dscl'),
    ds_record_type_extn = FOUR_CHAR_CODE('extn'),
    ds_record_type_fwi0 = FOUR_CHAR_CODE('fwi0'),
    ds_record_type_fwsw = FOUR_CHAR_CODE('fwsw'),
    ds_record_type_fwvh = FOUR_CHAR_CODE('fwvh'),
    ds_record_type_GRP0 = FOUR_CHAR_CODE('GRP0'),
    ds_record_type_icgo = FOUR_CHAR_CODE('icgo'),
    ds_record_type_icsp = FOUR_CHAR_CODE('icsp'),
    ds_record_type_icvo = FOUR_CHAR_CODE('icvo'),
    ds_record_type_icvp = FOUR_CHAR_CODE('icvp'),
    ds_record_type_icvP = FOUR_CHAR_CODE('icvP'),
    ds_record_type_icvt = FOUR_CHAR_CODE('icvt'),
    ds_record_type_info = FOUR_CHAR_CODE('info'),
    ds_record_type_logS = FOUR_CHAR_CODE('logS'), // prLogicalSize (logs)?
    ds_record_type_lg1S = FOUR_CHAR_CODE('lg1S'),
    ds_record_type_lssp = FOUR_CHAR_CODE('lssp'),
    ds_record_type_lsvo = FOUR_CHAR_CODE('lsvo'),
    ds_record_type_lsvt = FOUR_CHAR_CODE('lsvt'),
    ds_record_type_lsvp = FOUR_CHAR_CODE('lsvp'),
    ds_record_type_lsvP = FOUR_CHAR_CODE('lsvP'),
    ds_record_type_modD = FOUR_CHAR_CODE('modD'), // prModificationDate (modd)?
    ds_record_type_moDD = FOUR_CHAR_CODE('moDD'),
    ds_record_type_pBBk = FOUR_CHAR_CODE('pBBk'),
    ds_record_type_phyS = FOUR_CHAR_CODE('phyS'), // prPhysicalSize (phys)?
    ds_record_type_ph1S = FOUR_CHAR_CODE('ph1S'),
    ds_record_type_pict = FOUR_CHAR_CODE('pict'),
    ds_record_type_vSrn = FOUR_CHAR_CODE('vSrn'),
    ds_record_type_vstl = FOUR_CHAR_CODE('vstl')
} ds_record_type;

/*!
 * Data type, indicating what kind of data field the record stores.
 */
typedef enum {
    ds_record_data_type_long = FOUR_CHAR_CODE('long'),
    ds_record_data_type_shor = FOUR_CHAR_CODE('shor'),
    ds_record_data_type_bool = FOUR_CHAR_CODE('bool'),
    ds_record_data_type_blob = FOUR_CHAR_CODE('blob'),
    ds_record_data_type_type = FOUR_CHAR_CODE('type'),
    ds_record_data_type_ustr = FOUR_CHAR_CODE('ustr'),
    ds_record_data_type_comp = FOUR_CHAR_CODE('comp'),
    ds_record_data_type_dutc = FOUR_CHAR_CODE('dutc')
} ds_record_data_type;

AMG_EXPORT AMG_EXTERN ds_record_t *ds_record_create(void);
AMG_EXPORT AMG_EXTERN void ds_record_free(ds_record_t *record);

AMG_EXPORT AMG_EXTERN CFDictionaryRef ds_record_copy_dictionary(ds_record_t *record);

AMG_EXPORT AMG_EXTERN size_t ds_record_get_filename_len(ds_record_t *record);
AMG_EXPORT AMG_EXTERN void ds_record_copy_filename(ds_record_t *record, uint16_t *ustr);
AMG_EXPORT AMG_EXTERN const uint16_t *ds_record_get_filename_ptr(ds_record_t *record);

AMG_EXPORT AMG_EXTERN ds_record_type ds_record_get_type(ds_record_t *record);
AMG_EXPORT AMG_EXTERN ds_record_data_type ds_record_get_data_type(ds_record_t *record);

AMG_EXPORT AMG_EXTERN uint32_t ds_record_get_data_as_long(ds_record_t *record);
AMG_EXPORT AMG_EXTERN uint16_t ds_record_get_data_as_shor(ds_record_t *record);
AMG_EXPORT AMG_EXTERN bool ds_record_get_data_as_bool(ds_record_t *record);

AMG_EXPORT AMG_EXTERN void ds_record_copy_data_as_blob(ds_record_t *record, unsigned char *blob);
AMG_EXPORT AMG_EXTERN const unsigned char *ds_record_get_data_as_blob_ptr(ds_record_t *record);
AMG_EXPORT AMG_EXTERN size_t ds_record_get_data_as_blob_size(ds_record_t *record);

AMG_EXPORT AMG_EXTERN FourCharCode ds_record_get_data_as_type(ds_record_t *record);

AMG_EXPORT AMG_EXTERN void ds_copy_record_data_as_ustr(ds_record_t *record, uint16_t *ustr);
AMG_EXPORT AMG_EXTERN const uint16_t *ds_record_get_data_as_ustr_ptr(ds_record_t *record);
AMG_EXPORT AMG_EXTERN size_t ds_record_get_data_as_ustr_len(ds_record_t *record);

AMG_EXPORT AMG_EXTERN uint64_t ds_record_get_data_as_comp(ds_record_t *record);
AMG_EXPORT AMG_EXTERN UTCDateTime ds_record_get_data_as_dutc(ds_record_t *record);

AMG_EXPORT AMG_EXTERN CFPropertyListRef ds_record_get_data_as_plist(ds_record_t *record);

AMG_EXPORT AMG_EXTERN void ds_record_copy_data_as_plist_ustr(ds_record_t *record, uint16_t *ustr);
AMG_EXPORT AMG_EXTERN const uint16_t *ds_record_get_data_as_plist_ustr_ptr(ds_record_t *record);
AMG_EXPORT AMG_EXTERN size_t ds_record_get_data_as_plist_ustr_len(ds_record_t *record);

typedef struct { uint32_t x, y; unsigned char unknown[8]; } Iloc_t;
AMG_EXPORT AMG_EXTERN Iloc_t ds_record_get_data_as_Iloc(ds_record_t *record);

typedef struct { uint16_t top, left, bottom, right; uint32_t view; unsigned char unknown[4]; } fwi0_t;
AMG_EXPORT AMG_EXTERN fwi0_t ds_record_get_data_as_fwi0(ds_record_t *record);

typedef struct {
    uint32_t magic;
    uint32_t bookmark_size;
    uint32_t unknown; // 0x10040000
    uint32_t header_size;
    unsigned char reserved[32];

    uint32_t toc_offset;

    uint32_t toc_count; // API only
    struct {
        uint32_t size;
        uint32_t magic;
        uint32_t identifier;
        uint32_t next_toc_offset;
        uint32_t count;
        struct {
            uint32_t key;
            uint32_t offset;
            uint32_t reserved; // 0x000000
            struct {
                uint32_t length;
                uint32_t type;
                unsigned char value[256];
            } data;
        } entries[256];
    } toc[16];
} pBBk_t;

typedef enum {
    pBBk_string = 0x0101,
    pBBk_data = 0x0201,
    pBBk_int8 = 0x0301,
    pBBk_int16 = 0x0302,
    pBBk_int32 = 0x0303,
    pBBk_int64 = 0x0304,
    pBBk_float32 = 0x0305,
    pBBk_float64 = 0x0306,
    pBBk_date = 0x0400,
    pBBk_false = 0x0500,
    pBBk_true = 0x0501,
    pBBk_array = 0x0601,
    pBBk_dictionary = 0x0701,
    pBBk_uuid = 0x0801,
    pBBk_url = 0x0901,
    pBBk_url_relative = 0x0902,
} pBBK_data_type;

AMG_EXPORT AMG_EXTERN pBBk_t ds_record_get_data_as_pBBk(ds_record_t *record);
AMG_EXPORT AMG_EXTERN CFDictionaryRef _pBBk_record_copy_dictionary(const pBBk_t *);

AMG_EXPORT AMG_EXTERN pBBk_t _ds_get_data_as_pBBk(const unsigned char *data, size_t len);

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
    uint32_t metadata_count;
    struct {
        uint16_t tag;
        uint16_t length;
        unsigned char value[255]; // metadata entry value is naturally limited to 65535
        unsigned char padding; // unused
    } metadata_entries[255];
    // API is limited to 255 metadata entries for now; theoretical limit is 16346 because the
    // overall record size is a uint16, so 65535 bytes, minus the initial 150 bytes prior to the
    // metadata entries minus 16346 * 4 bytes (16346 entries each with a zero-length value) equals
    // 1 byte remaining. Limiting to 255 entries makes the structure size about 16 MB rather than
    // about 1 GB.
} pict_t;

AMG_EXPORT AMG_EXTERN pict_t _ds_get_data_as_pict(const unsigned char *data, size_t len);
AMG_EXPORT AMG_EXTERN CFDictionaryRef _pict_record_copy_dictionary(const pict_t *pictRecord);

AMG_EXPORT AMG_EXTERN pict_t ds_record_get_data_as_pict(ds_record_t *record);

#ifdef __cplusplus
AMG_EXPORT extern std::basic_string<uint16_t> ds_record_get_filename(ds_record_t *record);
AMG_EXPORT extern std::vector<unsigned char> ds_record_get_data_as_blob(ds_record_t *record);
AMG_EXPORT extern std::basic_string<uint16_t> ds_record_get_data_as_ustr(ds_record_t *record);
AMG_EXPORT extern std::basic_string<uint16_t> ds_record_get_data_as_plist_ustr(ds_record_t *record);
#endif

AMG_EXPORT AMG_EXTERN void ds_record_set_filename(ds_record_t *record, const uint16_t *ustr, size_t len);

AMG_EXPORT AMG_EXTERN void ds_record_set_type(ds_record_t *record, ds_record_type type);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_type(ds_record_t *record, ds_record_data_type type);

AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_long(ds_record_t *record, uint32_t llong);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_shor(ds_record_t *record, uint16_t shor);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_bool(ds_record_t *record, bool bbool);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_blob(ds_record_t *record, const unsigned char *blob, size_t len);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_type(ds_record_t *record, FourCharCode type);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_ustr(ds_record_t *record, const uint16_t *ustr, size_t len);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_comp(ds_record_t *record, uint64_t comp);
AMG_EXPORT AMG_EXTERN void ds_record_set_data_as_dutc(ds_record_t *record, UTCDateTime dutc);

#ifdef __cplusplus
AMG_EXPORT extern void ds_record_set_filename_obj(ds_record_t *record, const std::basic_string<uint16_t> &filename);
AMG_EXPORT extern void ds_record_set_data_as_blob_obj(ds_record_t *record, const std::vector<unsigned char> &blob);
AMG_EXPORT extern void ds_record_set_data_as_ustr_obj(ds_record_t *record, const std::basic_string<uint16_t> &ustr);
#endif

AMG_EXPORT AMG_EXTERN int ds_record_fread(ds_record_t *record, FILE *file);

AMG_EXPORT AMG_EXTERN ds_record_data_type ds_record_data_type_for_record_type(ds_record_type record_type);

void dsstore_record_BKGD_init(ds_record_t *record, ds_record_type recordType);

#endif // AMALGAMATE_DSRECORD_H
