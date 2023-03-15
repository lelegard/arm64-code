//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Comparative results on SHA-256 (portable vs. Arm64 instructions).
// Must be identical...
//
//----------------------------------------------------------------------------

#include "SHA256.h"
#include "ArmSHA256.h"
#include <ios>
#include <iomanip>
#include <iostream>

struct TestData {
    size_t size;
    uint8_t data[512];
    uint8_t hash[SHA256::HASH_SIZE];
};

static const TestData test_data[] = {
    {
        1,
        {0x12},
        {0xF2, 0x99, 0x79, 0x1C, 0xDD, 0xD3, 0xD6, 0x66, 0x4F, 0x66, 0x70, 0x84, 0x28, 0x12, 0xEF, 0x60,
         0x53, 0xEB, 0x65, 0x01, 0xBD, 0x62, 0x82, 0xA4, 0x76, 0xBB, 0xBF, 0x3E, 0xE9, 0x1E, 0x75, 0x0C}
    },
    {
        12,
        {0x76, 0x99, 0x53, 0x93, 0x3B, 0x67, 0xB0, 0xA5, 0xE3, 0x16, 0x39, 0xDE},
        {0x74, 0x63, 0xC4, 0xCE, 0x95, 0x7B, 0xA5, 0xD6, 0xB2, 0xD0, 0xAD, 0x3B, 0xDB, 0x09, 0x36, 0x32,
         0x2F, 0xF7, 0xEF, 0x93, 0x07, 0xEF, 0xDA, 0x99, 0x0F, 0x35, 0x48, 0x9F, 0xA2, 0xAC, 0x7D, 0x06}
    },
    {
        257,
        {0xDD, 0x37, 0xAA, 0x0E, 0xD6, 0x56, 0x99, 0xFA, 0x01, 0x10, 0x45, 0xAF, 0x3E, 0xB2, 0x7C, 0xD1,
         0x8D, 0x2B, 0x43, 0x24, 0x96, 0xD8, 0xF2, 0x55, 0xEE, 0x39, 0xC9, 0x86, 0x1D, 0x32, 0x54, 0xCF,
         0xAA, 0x00, 0x77, 0x87, 0x8D, 0xC6, 0x48, 0x2C, 0xB3, 0xFE, 0xB3, 0x11, 0x41, 0x1E, 0x96, 0x09,
         0x9F, 0x8B, 0xDA, 0xA4, 0x60, 0x8A, 0x8A, 0x3F, 0x2C, 0xB5, 0xE3, 0xA6, 0xC7, 0xA9, 0xFE, 0x4F,
         0x1C, 0xE2, 0x2D, 0xF1, 0x7C, 0x5F, 0x9E, 0xC9, 0x84, 0xA5, 0x4A, 0x87, 0x78, 0x1A, 0x88, 0xA2,
         0xB6, 0xC9, 0xAE, 0xB9, 0x14, 0x16, 0xB8, 0x7E, 0x5B, 0x59, 0xFA, 0x39, 0x36, 0xE4, 0x81, 0xFB,
         0x73, 0xA1, 0x69, 0x84, 0x87, 0xFA, 0x26, 0xA0, 0x0D, 0x27, 0x93, 0x7B, 0xBE, 0x14, 0xE8, 0x1F,
         0x5D, 0x76, 0xE1, 0xCB, 0x97, 0x87, 0xBB, 0x79, 0x3C, 0x65, 0x2D, 0xBA, 0x91, 0xB2, 0xAA, 0xFF,
         0xCB, 0x0B, 0xC4, 0xF9, 0x23, 0x20, 0x2C, 0x90, 0xA4, 0xFD, 0x99, 0x27, 0x32, 0x7C, 0xAA, 0xB9,
         0xFA, 0xE6, 0x6A, 0xEB, 0x43, 0x9B, 0x73, 0x53, 0x87, 0xCB, 0xC0, 0x6D, 0xC1, 0x68, 0x8D, 0x3F,
         0xD3, 0xE0, 0x6C, 0xDC, 0xB9, 0x3C, 0xF9, 0x06, 0xFF, 0x4E, 0x1D, 0x1E, 0x14, 0x79, 0xAF, 0xDD,
         0x8D, 0x18, 0xE7, 0x20, 0x43, 0xA5, 0x12, 0x60, 0x7D, 0x94, 0x60, 0x38, 0x5C, 0xC0, 0x45, 0xB6,
         0x9A, 0x0E, 0x74, 0xD1, 0xB3, 0x3A, 0x60, 0xB2, 0xCA, 0x5C, 0xBE, 0xD8, 0xC5, 0xA9, 0x21, 0x2D,
         0xFF, 0x66, 0x32, 0x0A, 0x57, 0x3D, 0x2B, 0x56, 0xED, 0x48, 0x5D, 0x2E, 0xF9, 0x05, 0xE7, 0x59,
         0x8C, 0xBC, 0xF8, 0x54, 0x52, 0xEA, 0xB1, 0xB5, 0x61, 0xD2, 0x5E, 0x7B, 0x23, 0xD9, 0x69, 0xFF,
         0xD2, 0xC4, 0xB8, 0x38, 0x48, 0x05, 0x54, 0x40, 0xC1, 0xE6, 0x3C, 0x64, 0xFB, 0x18, 0xBA, 0xF4,
         0x0C},
        {0x4E, 0x65, 0x59, 0x72, 0xAD, 0xFE, 0x4A, 0x80, 0x42, 0x52, 0x08, 0x52, 0x71, 0x57, 0x74, 0x94,
         0xA5, 0x12, 0x76, 0x41, 0xCD, 0xC3, 0x66, 0xC9, 0x25, 0xC4, 0x47, 0xC5, 0xBA, 0x58, 0x1D, 0x40}
    },
    {0, {}, {}}
};


//----------------------------------------------------------------------------
// Program entry point.
//----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    SHA256 sha;
    ArmSHA256 arm_sha;
    uint8_t hash[SHA256::HASH_SIZE];

    for (auto test = test_data; test->size > 0; ++test) {

        bzero(hash, sizeof(hash));
        sha.init();
        sha.add(test->data, test->size);
        sha.getHash(hash, sizeof(hash));
        const bool ok = ::memcmp(hash, test->hash, sizeof(hash)) == 0;

        bzero(hash, sizeof(hash));
        arm_sha.init();
        arm_sha.add(test->data, test->size);
        arm_sha.getHash(hash, sizeof(hash));
        const bool arm_ok = ::memcmp(hash, test->hash, sizeof(hash)) == 0;

        std::cout << std::setw(5) << test->size
                  << " bytes, SHA256: " << (ok ? "passed" : "FAILED")
                  << ", ArmSHA256: " << (arm_ok ? "passed" : "FAILED")
                  << std::endl;
    }

    return EXIT_SUCCESS;
}
