//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Implementation of CRC32 using Arm64 instructions for CRC32.
//
//----------------------------------------------------------------------------

#include "ArmCRC32.h"

// Arm Architecture Reference Manual, about the CRC32 instructions: "To align
// with common usage, the bit order of the values is reversed as part of the
// operation". However, the CRC32 computation for MPEG2-TS does not reverse
// the bits. Consequently, we have to reverse the bits again on input and
// output. We do this using 2 Arm64 instructions (would be dreadful in C++).

namespace {

    // Reverse all bits inside each individual byte of a 64-bit value.
    // Then, add the 64-bit result in the CRC32 computation.
    inline __attribute__((always_inline)) void crcAdd64(uint32_t& fcs, uint64_t x)
    {
        asm("rbit   %1, %1\n"
            "rev    %1, %1\n"
            "crc32x %w0, %w0, %1"
            : "+r" (fcs) : "r" (x));
    }

    // Same thing on one byte only.
    inline __attribute__((always_inline)) void crcAdd8(uint32_t& fcs, uint8_t x)
    {
        asm("rbit   %1, %1\n"
            "rev    %1, %1\n"
            "crc32b %w0, %w0, %w1"
            : "+r" (fcs) : "r" (uint64_t(x)));
    }
}

// Get the accumulated CRC32 value.
// Reverse the 32 bits in the result.
uint32_t ArmCRC32::value() const
{
    uint32_t x;
    asm("rbit %w0, %w1" : "=r" (x) : "r" (_fcs));
    return x;
}

void ArmCRC32::add(const void* data, size_t size)
{
    const uint8_t* cp8 = reinterpret_cast<const uint8_t*>(data);
    while (size != 0 && (uintptr_t(cp8) & 7) != 0) {
        crcAdd8(_fcs, *cp8++);
        --size;
    }

    const uint64_t* cp64 = reinterpret_cast<const uint64_t*>(cp8);
    while (size >= 64) {
        crcAdd64(_fcs, *cp64++);
        crcAdd64(_fcs, *cp64++);
        crcAdd64(_fcs, *cp64++);
        crcAdd64(_fcs, *cp64++);
        crcAdd64(_fcs, *cp64++);
        crcAdd64(_fcs, *cp64++);
        crcAdd64(_fcs, *cp64++);
        crcAdd64(_fcs, *cp64++);
        size -= 64;
    }
    while (size >= 8) {
        crcAdd64(_fcs, *cp64++);
        size -= 8;
    }

    cp8 = reinterpret_cast<const unsigned char*>(cp64);
    while (size--) {
        crcAdd8(_fcs, *cp8++);
    }
}
