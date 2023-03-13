//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Somme common definitions (see project TSDuck).
//
//----------------------------------------------------------------------------

#pragma once
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <algorithm>
#if defined(__linux__)
#include <byteswap.h>
#endif

inline __attribute__((always_inline)) uint32_t ByteSwap32(uint32_t x)
{
#if defined(__aarch64__) || defined(__arm64__)
    asm("rev %w0, %w0" : "+r" (x)); return x;
#elif defined(__linux__)
    return bswap_32(x);
#else
    return (x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24);
#endif
}

// Assume little endian
inline __attribute__((always_inline)) uint32_t GetUInt32(const void* p) { return ByteSwap32(*(static_cast<const uint32_t*>(p))); }
inline __attribute__((always_inline)) void PutUInt32(void* p, uint32_t i) { *(static_cast<uint32_t*>(p)) = ByteSwap32(i); }

inline __attribute__((always_inline)) uint32_t ROLc(uint32_t word, const int i)
{
#if !defined(__aarch64__) && !defined(__arm64__)
    return ((word << (i&31)) | ((word&0xFFFFFFFFUL) >> (32-(i&31)))) & 0xFFFFFFFFUL;
#elif defined(DEBUG)
    asm("mov w8, #32 \n sub w8, w8, %w1 \n ror %w0, %w0, w8" : "+r" (word) : "r" (i) : "w8", "cc");
#else
    asm("ror %w0, %w0, %1" : "+r" (word) : "I" (32-i));
#endif
    return word;
}

inline __attribute__((always_inline)) uint32_t RORc(uint32_t word, const int i)
{
#if !defined(__aarch64__) && !defined(__arm64__)
    return (((word&0xFFFFFFFFUL) >> (i&31)) | (word << (32-(i&31)))) & 0xFFFFFFFFUL;
#elif defined(DEBUG)
    asm("ror %w0, %w0, %w1" : "+r" (word) : "r" (i));
    return word;
#else
    asm("ror %w0, %w0, %1" : "+r" (word) : "I" (i));
    return word;
#endif
}
