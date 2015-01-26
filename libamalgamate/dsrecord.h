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
