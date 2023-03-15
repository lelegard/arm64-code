//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Implementation of SHA-256 using Arm64 instructions.
//
//----------------------------------------------------------------------------

#pragma once
#include "platform.h"

class ArmSHA256
{
public:
    static const size_t HASH_SIZE  = 32;  //!< SHA-256 hash size in bytes.
    static const size_t BLOCK_SIZE = 64;  //!< SHA-256 block size in bytes.

    ArmSHA256();
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
