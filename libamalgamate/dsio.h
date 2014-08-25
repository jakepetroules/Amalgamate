//
//  dsio.h
//  Amalgamate
//
//  Created by Jake Petroules on 2014-08-24.
//
//

#ifndef Amalgamate_dsio_h
#define Amalgamate_dsio_h

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <arpa/inet.h>

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
