//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Implementation of SHA-256 using Arm64 instructions.
// Based on public domain code from Arm.
//
//----------------------------------------------------------------------------

#include "ArmSHA256.h"
#include <arm_neon.h>


//----------------------------------------------------------------------------
// The K array
//----------------------------------------------------------------------------

namespace {
    const uint32_t K[64] = {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
        0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
        0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
        0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
        0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
        0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
        0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
        0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
        0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
        0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
    };
}


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ArmSHA256::ArmSHA256() :
    _length(0),
    _curlen(0)
{
    init();
}


//----------------------------------------------------------------------------
// Reinitialize the computation of the hash.
//----------------------------------------------------------------------------

bool ArmSHA256::init()
{
    _curlen = 0;
    _length = 0;
    _state[0] = 0x6A09E667UL;
    _state[1] = 0xBB67AE85UL;
    _state[2] = 0x3C6EF372UL;
    _state[3] = 0xA54FF53AUL;
    _state[4] = 0x510E527FUL;
    _state[5] = 0x9B05688CUL;
    _state[6] = 0x1F83D9ABUL;
    _state[7] = 0x5BE0CD19UL;
    return true;
}


//----------------------------------------------------------------------------
// Compress part of message
//----------------------------------------------------------------------------

void ArmSHA256::compress(const uint8_t* buf)
{
    // Load initial values.
    uint32x4_t state0 = vld1q_u32(&_state[0]);
    uint32x4_t state1 = vld1q_u32(&_state[4]);

    // Save current state.
    const uint32x4_t previous_state0 = state0;
    const uint32x4_t previous_state1 = state1;

    // Load input block.
    const uint32_t* buf32 = reinterpret_cast<const uint32_t*>(buf);
    uint32x4_t msg0 = vld1q_u32(buf32 + 0);
    uint32x4_t msg1 = vld1q_u32(buf32 + 4);
    uint32x4_t msg2 = vld1q_u32(buf32 + 8);
    uint32x4_t msg3 = vld1q_u32(buf32 + 12);

    // Swap bytes on little endian Arm64.
    msg0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(msg0)));
    msg1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(msg1)));
    msg2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(msg2)));
    msg3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(msg3)));

    // Rounds 0-3
    uint32x4_t msg_k = vaddq_u32(msg0, vld1q_u32(&K[4*0]));
    uint32x4_t tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg0 = vsha256su1q_u32(vsha256su0q_u32(msg0, msg1), msg2, msg3);

    // Rounds 4-7
    msg_k = vaddq_u32(msg1, vld1q_u32(&K[4*1]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg1 = vsha256su1q_u32(vsha256su0q_u32(msg1, msg2), msg3, msg0);

    // Rounds 8-11
    msg_k = vaddq_u32(msg2, vld1q_u32(&K[4*2]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg2 = vsha256su1q_u32(vsha256su0q_u32(msg2, msg3), msg0, msg1);

    // Rounds 12-15
    msg_k = vaddq_u32(msg3, vld1q_u32(&K[4*3]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg3 = vsha256su1q_u32(vsha256su0q_u32(msg3, msg0), msg1, msg2);

    // Rounds 16-19
    msg_k = vaddq_u32(msg0, vld1q_u32(&K[4*4]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg0 = vsha256su1q_u32(vsha256su0q_u32(msg0, msg1), msg2, msg3);

    // Rounds 20-23
    msg_k = vaddq_u32(msg1, vld1q_u32(&K[4*5]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg1 = vsha256su1q_u32(vsha256su0q_u32(msg1, msg2), msg3, msg0);

    // Rounds 24-27
    msg_k = vaddq_u32(msg2, vld1q_u32(&K[4*6]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg2 = vsha256su1q_u32(vsha256su0q_u32(msg2, msg3), msg0, msg1);

    // Rounds 28-31
    msg_k = vaddq_u32(msg3, vld1q_u32(&K[4*7]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg3 = vsha256su1q_u32(vsha256su0q_u32(msg3, msg0), msg1, msg2);

    // Rounds 32-35
    msg_k = vaddq_u32(msg0, vld1q_u32(&K[4*8]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg0 = vsha256su1q_u32(vsha256su0q_u32(msg0, msg1), msg2, msg3);

    // Rounds 36-39
    msg_k = vaddq_u32(msg1, vld1q_u32(&K[4*9]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg1 = vsha256su1q_u32(vsha256su0q_u32(msg1, msg2), msg3, msg0);

    // Rounds 40-43
    msg_k = vaddq_u32(msg2, vld1q_u32(&K[4*10]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg2 = vsha256su1q_u32(vsha256su0q_u32(msg2, msg3), msg0, msg1);

    // Rounds 44-47
    msg_k = vaddq_u32(msg3, vld1q_u32(&K[4*11]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;
    msg3 = vsha256su1q_u32(vsha256su0q_u32(msg3, msg0), msg1, msg2);

    // Rounds 48-51
    msg_k = vaddq_u32(msg0, vld1q_u32(&K[4*12]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;

    // Rounds 52-55
    msg_k = vaddq_u32(msg1, vld1q_u32(&K[4*13]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;

    // Rounds 56-59
    msg_k = vaddq_u32(msg2, vld1q_u32(&K[4*14]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;

    // Rounds 60-63
    msg_k = vaddq_u32(msg3, vld1q_u32(&K[4*15]));
    tmp_state = vsha256hq_u32(state0, state1, msg_k);
    state1 = vsha256h2q_u32(state1, state0, msg_k);
    state0 = tmp_state;

    // Add back to state
    state0 = vaddq_u32(state0, previous_state0);
    state1 = vaddq_u32(state1, previous_state1);

    // Save state
    vst1q_u32(&_state[0], state0);
    vst1q_u32(&_state[4], state1);
}


//----------------------------------------------------------------------------
// Add some part of the message to hash. Can be called several times.
//----------------------------------------------------------------------------

bool ArmSHA256::add(const void* data, size_t size)
{
    // Filter invalid internal state.
    if (_curlen >= sizeof(_buf)) {
        return false;
    }

    const uint8_t* in = reinterpret_cast<const uint8_t*>(data);
    while (size > 0) {
        if (_curlen == 0 && size >= BLOCK_SIZE) {
            // Compress one 512-bit block directly from user's buffer.
            compress(in);
            _length += BLOCK_SIZE * 8;
            in += BLOCK_SIZE;
            size -= BLOCK_SIZE;
        }
        else {
            // Partial block, Accumulate input data in internal buffer.
            size_t n = std::min(size, (BLOCK_SIZE - _curlen));
            ::memcpy(_buf + _curlen, in, n);
            _curlen += n;
            in += n;
            size -= n;
            if (_curlen == BLOCK_SIZE) {
                compress(_buf);
                _length += 8 * BLOCK_SIZE;
                _curlen = 0;
            }
        }
    }
    return true;
}


//----------------------------------------------------------------------------
// Get the resulting hash value.
//----------------------------------------------------------------------------

bool ArmSHA256::getHash(void* hash, size_t bufsize, size_t* retsize)
{
    // Filter invalid internal state or invalid input.
    if (_curlen >= sizeof(_buf) || bufsize < HASH_SIZE) {
        return false;
    }

    // Increase the length of the message
    _length += _curlen * 8;

    // Append the '1' bit
    _buf[_curlen++] = 0x80;

    // Pad with zeroes and append 64-bit message length in bits.
    // If the length is currently above 56 bytes (no room for message length), append zeroes then compress.
    if (_curlen > 56) {
        bzero(_buf + _curlen, 64 - _curlen);
        compress(_buf);
        _curlen = 0;
    }

    // Pad up to 56 bytes with zeroes and append 64-bit message length in bits.
    bzero(_buf + _curlen, 56 - _curlen);
    PutUInt64(_buf + 56, _length);
    compress(_buf);

    // Copy output
    uint8_t* out = reinterpret_cast<uint8_t*> (hash);
    for (size_t i = 0; i < 8; i++) {
        PutUInt32(out + 4*i, _state[i]);
    }

    if (retsize != nullptr) {
        *retsize = HASH_SIZE;
    }
    return true;
}
