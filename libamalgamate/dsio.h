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

inline static uint64_t uint64_from_be(const unsigned char *p) {
    assert(p);
    uint64_t i;
    memcpy(&i, p, sizeof(i));
    return ntohll(i);
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

#endif
