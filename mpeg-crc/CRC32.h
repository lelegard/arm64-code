//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Portable implementation of CRC32 (see project TSDuck).
//
//----------------------------------------------------------------------------

#pragma once
#include <cstdlib>
#include <cinttypes>

class CRC32
{
public:
    CRC32(uint32_t init = 0xFFFFFFFF) : _fcs(init) {}
    void reset(uint32_t init = 0xFFFFFFFF) { _fcs = init; }
    void add(const void* data, size_t size);
    uint32_t value() const { return _fcs; }
private:
    uint32_t _fcs;
    static const uint32_t _fcstab_32[256];
};
