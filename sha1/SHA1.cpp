//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Portable implementation of SHA-1 (see project TSDuck).
//
//----------------------------------------------------------------------------

#include "SHA1.h"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

SHA1::SHA1() :
    _length(0),
    _curlen(0)
{
    init();
}


//----------------------------------------------------------------------------
// Reinitialize the computation of the hash.
// Return true on success, false on error.
//----------------------------------------------------------------------------

bool SHA1::init()
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

void SHA1::compress(const uint8_t* buf)
{
    // Copy state
    uint32_t a = _state[0];
    uint32_t b = _state[1];
    uint32_t c = _state[2];
    uint32_t d = _state[3];
    uint32_t e = _state[4];

    // Copy input block (512 bits, 64 bytes, 16 uint32) into W[0..15]
    uint32_t i, W[80];
    for (i = 0; i < 16; i++) {
        W[i] = GetUInt32(buf + 4*i);
    }

    // Expand it over 320 bytes (80 uint32)
    for (i = 16; i < 80; i++) {
        W[i] = ROLc(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1);
    }

    // Compress
    #define F0(x,y,z) (z ^ (x & (y ^ z)))
    #define F1(x,y,z) (x ^ y ^ z)
    #define F2(x,y,z) ((x & y) | (z & (x | y)))
    #define F3(x,y,z) (x ^ y ^ z)

    #define FF0(a,b,c,d,e,i) e = (ROLc(a, 5) + F0(b,c,d) + e + W[i] + 0x5a827999UL); b = ROLc(b, 30)
    #define FF1(a,b,c,d,e,i) e = (ROLc(a, 5) + F1(b,c,d) + e + W[i] + 0x6ed9eba1UL); b = ROLc(b, 30)
    #define FF2(a,b,c,d,e,i) e = (ROLc(a, 5) + F2(b,c,d) + e + W[i] + 0x8f1bbcdcUL); b = ROLc(b, 30)
    #define FF3(a,b,c,d,e,i) e = (ROLc(a, 5) + F3(b,c,d) + e + W[i] + 0xca62c1d6UL); b = ROLc(b, 30)

    // Round one
    i = 0;
    while (i < 20) {
        FF0(a,b,c,d,e,i++);
        FF0(e,a,b,c,d,i++);
        FF0(d,e,a,b,c,i++);
        FF0(c,d,e,a,b,i++);
        FF0(b,c,d,e,a,i++);
    }

    // Round two
    while (i < 40) {
        FF1(a,b,c,d,e,i++);
        FF1(e,a,b,c,d,i++);
        FF1(d,e,a,b,c,i++);
        FF1(c,d,e,a,b,i++);
        FF1(b,c,d,e,a,i++);
    }

    // Round three
    while (i < 60) {
        FF2(a,b,c,d,e,i++);
        FF2(e,a,b,c,d,i++);
        FF2(d,e,a,b,c,i++);
        FF2(c,d,e,a,b,i++);
        FF2(b,c,d,e,a,i++);
    }

    // Round four
    while (i < 80) {
        FF3(a,b,c,d,e,i++);
        FF3(e,a,b,c,d,i++);
        FF3(d,e,a,b,c,i++);
        FF3(c,d,e,a,b,i++);
        FF3(b,c,d,e,a,i++);
    }

    #undef FF0
    #undef FF1
    #undef FF2
    #undef FF3

    #undef F0
    #undef F1
    #undef F2
    #undef F3

    // Store
    _state[0] += a;
    _state[1] += b;
    _state[2] += c;
    _state[3] += d;
    _state[4] += e;
}


//----------------------------------------------------------------------------
// Add some part of the message to hash. Can be called several times.
// Return true on success, false on error.
//----------------------------------------------------------------------------

bool SHA1::add(const void* data, size_t size)
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

bool SHA1::getHash(void* hash, size_t bufsize, size_t* retsize)
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
