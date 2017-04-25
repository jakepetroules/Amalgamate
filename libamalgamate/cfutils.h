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

#ifndef cfutils_h
#define cfutils_h

#include "amgmemory.h"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

static void AMGCFDictionarySetSInt8Value(CFMutableDictionaryRef dict, CFStringRef key, int8_t value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberSInt8Type,
                                                                 &value)));

}

static void AMGCFDictionarySetSInt16Value(CFMutableDictionaryRef dict, CFStringRef key, int16_t value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberSInt16Type,
                                                                 &value)));

}

static void AMGCFDictionarySetSInt32Value(CFMutableDictionaryRef dict, CFStringRef key, int32_t value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberSInt32Type,
                                                                 &value)));

}

static void AMGCFDictionarySetSInt64Value(CFMutableDictionaryRef dict, CFStringRef key, int64_t value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberSInt64Type,
                                                                 &value)));

}

static void AMGCFDictionarySetFloat32Value(CFMutableDictionaryRef dict, CFStringRef key, float value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberFloat32Type,
                                                                 &value)));

}

static void AMGCFDictionarySetFloat64Value(CFMutableDictionaryRef dict, CFStringRef key, double value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberFloat64Type,
                                                                 &value)));

}

static void AMGCFDictionarySetFourCCValue(CFMutableDictionaryRef dict, CFStringRef key, uint32_t value)
{
    const uint32_t value_n = htonl(value);
    const char *value_ptr = reinterpret_cast<const char *>(&value_n);

    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFStringRef>(CFStringCreateWithFormat(kCFAllocatorDefault,
                                                                           NULL, CFSTR("%c%c%c%c"),
                                                                           value_ptr[0],
                                                                           value_ptr[1],
                                                                           value_ptr[2],
                                                                           value_ptr[3])));
}

static void AMGCFDictionarySetShortValue(CFMutableDictionaryRef dict, CFStringRef key, uint16_t value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberShortType,
                                                                 &value)));

}

static void AMGCFDictionarySetIntValue(CFMutableDictionaryRef dict, CFStringRef key, uint32_t value)
{
    CFDictionarySetValue(dict, key,
                         AMCFTypeRef<CFNumberRef>(CFNumberCreate(kCFAllocatorDefault,
                                                                 kCFNumberIntType,
                                                                 &value)));

}

static CFStringRef AMGCFStringCreateCString(const char *s, size_t len)
{
    return CFStringCreateWithBytes(kCFAllocatorDefault,
                                   reinterpret_cast<const UInt8 *>(s),
                                   static_cast<CFIndex>(len),
                                   kCFStringEncodingMacRoman, 0);
}

static CFStringRef AMGCFStringCreateUTF8String(const char *s, size_t len)
{
    return CFStringCreateWithBytes(kCFAllocatorDefault,
                                   reinterpret_cast<const UInt8 *>(s),
                                   static_cast<CFIndex>(len),
                                   kCFStringEncodingUTF8, 0);
}

static CFStringRef AMGCFStringCreateHFSString(const char *s, size_t len)
{
    return CFStringCreateWithBytes(kCFAllocatorDefault,
                                   reinterpret_cast<const UInt8 *>(s),
                                   static_cast<CFIndex>(len),
                                   kCFStringEncodingMacHFS, 0);
}

static void AMGCFDictionarySetCStringValue(CFMutableDictionaryRef dict, CFStringRef key, const char *s, size_t len)
{
    CFDictionarySetValue(dict, key, AMCFTypeRef<CFStringRef>(AMGCFStringCreateCString(s, len)));
}

static void AMGCFDictionarySetUTF8StringValue(CFMutableDictionaryRef dict, CFStringRef key, const char *s, size_t len)
{
    CFDictionarySetValue(dict, key, AMCFTypeRef<CFStringRef>(AMGCFStringCreateUTF8String(s, len)));
}

static void AMGCFDictionarySetHFSStringValue(CFMutableDictionaryRef dict, CFStringRef key, const char *s, size_t len)
{
    CFDictionarySetValue(dict, key, AMCFTypeRef<CFStringRef>(AMGCFStringCreateHFSString(s, len)));
}

static void AMGCFDictionarySetMacDateValue(CFMutableDictionaryRef dict, CFStringRef key, uint32_t value)
{
    CFDictionarySetValue(dict, key, AMCFTypeRef<CFDateRef>(CFDateCreate(kCFAllocatorDefault, value - kCFAbsoluteTimeIntervalSince1904)));
}

static void AMGCFDictionarySetCFDateValue(CFMutableDictionaryRef dict, CFStringRef key, CFAbsoluteTime cftime)
{
    CFDictionarySetValue(dict, key, AMCFTypeRef<CFDateRef>(CFDateCreate(kCFAllocatorDefault, cftime)));
}

static void AMGCFDictionarySetUTCDateValue(CFMutableDictionaryRef dict, CFStringRef key, const UTCDateTime *dt)
{
    CFAbsoluteTime cftime;
    UCConvertUTCDateTimeToCFAbsoluteTime(dt, &cftime);
    AMGCFDictionarySetCFDateValue(dict, key, cftime);
}

#endif /* cfutils_h */
