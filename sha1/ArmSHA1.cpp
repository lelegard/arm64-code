//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Implementation of SHA-1 using Arm64 instructions.
// Based on public domain code from Arm.
//
//----------------------------------------------------------------------------

#include "ArmSHA1.h"
#include <arm_neon.h>


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ArmSHA1::ArmSHA1() :
    _length(0),
    _curlen(0)
{
    init();
}


//----------------------------------------------------------------------------
// Reinitialize the computation of the hash.
// Return true on success, false on error.
//----------------------------------------------------------------------------

bool ArmSHA1::init()
{
    _state[0] = 0x67452301UL;
    _state[1] = 0xEFCDAB89UL;
    _state[2] = 0x98BADCFEUL;
    _state[3] = 0x10325476UL;
    _state[4] = 0xC3D2E1F0UL;
    _curlen = 0;
    _length = 0;
    return true;
}


//----------------------------------------------------------------------------
// Compress part of message
//----------------------------------------------------------------------------

void ArmSHA1::compress(const uint8_t* buf)
{
    // Copy state
    uint32x4_t ABCD = vld1q_u32(_state);
    uint32_t E = _state[4];

    const uint32x4_t C0 = vdupq_n_u32(0x5A827999);
    const uint32x4_t C1 = vdupq_n_u32(0x6ED9EBA1);
    const uint32x4_t C2 = vdupq_n_u32(0x8F1BBCDC);
    const uint32x4_t C3 = vdupq_n_u32(0xCA62C1D6);

    const uint32_t* buf32 = reinterpret_cast<const uint32_t*>(buf);
    uint32x4_t MSG0 = vld1q_u32(buf32 + 0);
    uint32x4_t MSG1 = vld1q_u32(buf32 + 4);
    uint32x4_t MSG2 = vld1q_u32(buf32 + 8);
    uint32x4_t MSG3 = vld1q_u32(buf32 + 12);

    MSG0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG0)));
    MSG1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG1)));
    MSG2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG2)));
    MSG3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG3)));

    uint32x4_t TMP0 = vaddq_u32(MSG0, C0);
    uint32x4_t TMP1 = vaddq_u32(MSG1, C0);

    // Rounds 0-3
    uint32_t E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG2, C0);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    // Rounds 4-7
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, C0);
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    // Rounds 8-11
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG0, C0);
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    // Rounds 12-15
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, C1);
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    // Rounds 16-19
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG2, C1);
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    // Rounds 20-23
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, C1);
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    // Rounds 24-27
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG0, C1);
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    // Rounds 28-31
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, C1);
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    // Rounds 32-35
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG2, C2);
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    // Rounds 36-39
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, C2);
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    // Rounds 40-43
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG0, C2);
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    // Rounds 44-47
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, C2);
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    // Rounds 48-51
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG2, C2);
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    // Rounds 52-55
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, C3);
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    // Rounds 56-59
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG0, C3);
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    // Rounds 60-63
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, C3);
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    // Rounds 64-67
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E, TMP0);
    TMP0 = vaddq_u32(MSG2, C3);
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    // MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    // Rounds 68-71
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, C3);
    // MSG0 = vsha1su1q_u32(MSG0, MSG3);

    // Rounds 72-75
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E, TMP0);

    // Rounds 76-79
    E = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);

    // Store state: add ABCD E to state 0..5 
    vst1q_u32(_state, vaddq_u32(vld1q_u32(_state), ABCD));
    _state[4] += E;
}


//----------------------------------------------------------------------------
// Add some part of the message to hash. Can be called several times.
// Return true on success, false on error.
//----------------------------------------------------------------------------

bool ArmSHA1::add(const void* data, size_t size)
{
    if (_curlen >= sizeof(_buf)) {
        return false; // invalid internal state
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
            const size_t n = std::min(size, (BLOCK_SIZE - _curlen));
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
// If retsize is non-zero, return the actual hash size.
// Return true on success, false on error.
//----------------------------------------------------------------------------

bool ArmSHA1::getHash(void* hash, size_t bufsize, size_t* retsize)
{
    if (_curlen >= sizeof(_buf) || bufsize < HASH_SIZE) {
        return false; // invalid internal state or invalid input
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
    uint8_t* out = reinterpret_cast<uint8_t*>(hash);
    PutUInt32(out, _state[0]);
    PutUInt32(out + 4, _state[1]);
    PutUInt32(out + 8, _state[2]);
    PutUInt32(out + 12, _state[3]);
    PutUInt32(out + 16, _state[4]);
    
    if (retsize != nullptr) {
        *retsize = HASH_SIZE;
    }
    return true;
}
