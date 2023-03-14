//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Implementation of CRC32 using Arm64 instructions for CRC32.
//
//----------------------------------------------------------------------------

#pragma once
#include <cstdlib>
#include <cinttypes>

class ArmCRC32
{
public:
    ArmCRC32(uint32_t init = 0xFFFFFFFF) : _fcs(init) {}
    void reset(uint32_t init = 0xFFFFFFFF) { _fcs = init; }
    void add(const void* data, size_t size);
    uint32_t value() const;
private:
    uint32_t _fcs;
};
