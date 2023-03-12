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
    static const size_t HASH_SIZE  = 20;
    static const size_t BLOCK_SIZE = 64;

    SHA1();
    bool init();
    bool add(const void* data, size_t size);
    bool getHash(void* hash, size_t bufsize, size_t* retsize = nullptr);

private:
    void compress(const uint8_t* buf);
    uint64_t _length;
    uint32_t _state[HASH_SIZE / 4];
    size_t   _curlen;
    uint8_t  _buf[BLOCK_SIZE];
};
