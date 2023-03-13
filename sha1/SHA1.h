//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Portable implementation of SHA-1 (see project TSDuck).
//
//----------------------------------------------------------------------------

#pragma once
#include "platform.h"

class SHA1
{
public:
    static const size_t HASH_SIZE  = 20;  // 160 bits
    static const size_t BLOCK_SIZE = 64;  // 512 bits

    SHA1();
    bool init();
    bool add(const void* data, size_t size);
    bool getHash(void* hash, size_t bufsize, size_t* retsize = nullptr);

private:
    uint64_t _length;                 // Total message size in bits (already hashed, ie. excluding _buf)
    uint32_t _state[HASH_SIZE / 4];   // Current hash value (160 bits)
    size_t   _curlen;                 // Used bytes in _buf
    uint8_t  _buf[BLOCK_SIZE];        // Current block to hash (512 bits)

    // Compress one 512-bit block, accumulate hash in _state.
    void compress(const uint8_t* buf);
};
