//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Implementation of SHA-512 using Arm64 instructions.
//
//----------------------------------------------------------------------------

#pragma once
#include "platform.h"

class ArmSHA512
{
public:
    static const size_t HASH_SIZE  =  64;  //!< SHA-512 hash size in bytes (512 bits).
    static const size_t BLOCK_SIZE = 128;  //!< SHA-512 block size in bytes (1024 bits).

    ArmSHA512();
    bool init();
    bool add(const void* data, size_t size);
    bool getHash(void* hash, size_t bufsize, size_t* retsize = nullptr);

private:
    uint64_t _length;                // Total message size in bits (already hashed, ie. excluding _buf)
    size_t   _curlen;                // Used bytes in _buf
    uint64_t _state[HASH_SIZE / 8];  // Current hash value (512 bits, 64 bytes, 8 uint64)
    uint8_t  _buf[BLOCK_SIZE];       // Current block to hash (1024 bits, 128 bytes)

    void compress(const uint8_t* buf);
};
