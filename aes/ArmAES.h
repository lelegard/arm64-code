//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Implementation of AES using Arm64 instructions.
//
//----------------------------------------------------------------------------

#pragma once
#include "platform.h"
#include <arm_neon.h>

class ArmAES
{
 public:
    ArmAES();                                     //!< Constructor.
    static constexpr size_t BLOCK_SIZE = 16;      //!< AES block size in bytes.
    static constexpr size_t MIN_KEY_SIZE = 16;    //!< AES minimum key size in bytes.
    static constexpr size_t MAX_KEY_SIZE = 32;    //!< AES maximum key size in bytes.
    static constexpr size_t MIN_ROUNDS = 10;      //!< AES minimum number of rounds.
    static constexpr size_t MAX_ROUNDS = 14;      //!< AES maximum number of rounds.
    static constexpr size_t DEFAULT_ROUNDS = 10;  //!< AES default number of rounds, actually depends on key size.

    bool setKey(const void* key, size_t key_length);
    bool encrypt(const void* plain, size_t plain_length, void* cipher, size_t cipher_maxsize, size_t* cipher_length);
    bool decrypt(const void* cipher, size_t cipher_length, void* plain, size_t plain_maxsize, size_t* plain_length);

 private:
    size_t     _kbits;
    int        _Nr;      // Number of rounds
    uint32_t   _eK[60];  // Scheduled encryption keys
    uint32_t   _dK[60];  // Scheduled decryption keys
    uint8x16_t _aeK[15];
    uint8x16_t _adK[15];
};
