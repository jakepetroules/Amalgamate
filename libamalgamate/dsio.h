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

#ifndef Amalgamate_dsio_h
#define Amalgamate_dsio_h

#include <assert.h>
#include <memory.h>
#include <stddef.h>
#include <stdio.h>
#include <arpa/inet.h>

#define ltohs(n) (ntohs(0x0102) == 0x0102 ? _OSSwapInt32(n) : n)
#define ltohl(n) (ntohl(0x01020304) == 0x01020304 ? _OSSwapInt32(n) : n)
#define ltohll(n) (ntohl(0x0102030405060708) == 0x0102030405060708 ? _OSSwapInt32(n) : n)

inline static uint8_t uint8_from(const unsigned char *p) {
    assert(p);
    uint8_t i;
    memcpy(&i, p, sizeof(i));
    return i;
}

inline static uint16_t uint16_from_be(const unsigned char *p) {
    assert(p);
    uint16_t i;
    memcpy(&i, p, sizeof(i));
    return ntohs(i);
}

inline static uint32_t uint32_from_be(const unsigned char *p) {
    assert(p);
    uint32_t i;
    memcpy(&i, p, sizeof(i));
    return ntohl(i);
}

inline static uint32_t uint32_from_le(const unsigned char *p) {
    assert(p);
    uint32_t i;
    memcpy(&i, p, sizeof(i));
    return ltohl(i);
}

inline static uint64_t uint64_from_be(const unsigned char *p) {
    assert(p);
    uint64_t i;
    memcpy(&i, p, sizeof(i));
    return ntohll(i);
}

inline static const unsigned char *read_uint8_from(const unsigned char *p, uint8_t *out) {
    uint8_t value = uint8_from(p);
    if (out)
        *out = value;
    return p + sizeof(value);
}

inline static const unsigned char *read_int16_from_be(const unsigned char *p, int16_t *out) {
    uint16_t value = uint16_from_be(p);
    if (out)
        *out = (int16_t)value;
    return p + sizeof(value);
}

inline static const unsigned char *read_uint16_from_be(const unsigned char *p, uint16_t *out) {
    uint16_t value = uint16_from_be(p);
    if (out)
        *out = value;
    return p + sizeof(value);
}

inline static const unsigned char *read_uint32_from_be(const unsigned char *p, uint32_t *out) {
    uint32_t value = uint32_from_be(p);
    if (out)
        *out = value;
    return p + sizeof(value);
}

inline static const unsigned char *read_uint32_from_le(const unsigned char *p, uint32_t *out) {
    uint32_t value = uint32_from_le(p);
    if (out)
        *out = value;
    return p + sizeof(value);
}

inline static const unsigned char *read_uint64_from_be(const unsigned char *p, uint64_t *out) {
    uint64_t value = uint64_from_be(p);
    if (out)
        *out = value;
    return p + sizeof(value);
}

inline static const unsigned char *read_data_from(const unsigned char *p, unsigned char *out, size_t size) {
    memcpy(out, p, size);
    return p + size;
}

inline static size_t fread_uint8(uint8_t *value, FILE *file)
{
    assert(value);
    assert(file);
    return fread(value, sizeof(*value), 1, file);
}

inline static size_t fread_uint16_be(uint16_t *value, FILE *file)
{
    assert(value);
    assert(file);
    size_t count = fread(value, sizeof(*value), 1, file);
    *value = ntohs(*value);
    return count;
}

inline static size_t fread_uint32_be(uint32_t *value, FILE *file)
{
    assert(value);
    assert(file);
    size_t count = fread(value, sizeof(*value), 1, file);
    *value = ntohl(*value);
    return count;
}

inline static size_t fread_uint64_be(uint64_t *value, FILE *file)
{
    assert(value);
    assert(file);
    size_t count = fread(value, sizeof(*value), 1, file);
    *value = ntohll(*value);
    return count;
}

inline static size_t fwrite_uint8(const uint8_t *value, FILE *file)
{
    assert(value);
    assert(file);
    return fwrite(value, sizeof(*value), 1, file);
}

inline static size_t fwrite_uint16_be(const uint16_t *value, FILE *file)
{
    assert(value);
    assert(file);
    const uint16_t value_n = htons(*value);
    return fwrite(&value_n, sizeof(value_n), 1, file);
}

inline static size_t fwrite_uint32_be(const uint32_t *value, FILE *file)
{
    assert(value);
    assert(file);
    const uint32_t value_n = htonl(*value);
    return fwrite(&value_n, sizeof(value_n), 1, file);
}

inline static size_t fwrite_uint64_be(uint64_t *value, FILE *file)
{
    assert(value);
    assert(file);
    const uint64_t value_n = htonll(*value);
    return fwrite(&value_n, sizeof(value_n), 1, file);
}

#undef ltohs
#undef ltohl
#undef ltohll

#endif
