//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Comparative performance test on SHA-512 (portable vs. Arm64 instructions).
// Specify the number of iterations on the command line.
//
//----------------------------------------------------------------------------

#include "SHA512.h"
#include "ArmSHA512.h"
#include <ios>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <sys/resource.h>

#define DEFAULT_ITERATIONS 10000000

static const uint8_t test_data[256] = {
    0x8F, 0xAA, 0xF6, 0x60, 0x79, 0x8C, 0x25, 0x3A, 0xF7, 0x51, 0x5D, 0x80, 0x8B, 0x3F, 0x7D, 0x71,
    0xAF, 0x18, 0xD8, 0x15, 0x57, 0x97, 0xD9, 0xFB, 0x89, 0x94, 0x4A, 0x46, 0x87, 0x4A, 0xF1, 0x16,
    0xF0, 0xA8, 0x93, 0x25, 0xB0, 0x90, 0xE0, 0x19, 0xDD, 0x2F, 0xA1, 0x6B, 0x7D, 0xB0, 0x6D, 0x4D,
    0xE8, 0x2F, 0x3F, 0x0B, 0x1A, 0x71, 0x03, 0x13, 0xE3, 0xB8, 0x37, 0xBA, 0x2C, 0xA4, 0x07, 0xB4,
    0xBD, 0x94, 0xFE, 0xDC, 0x17, 0xE0, 0xA6, 0x1A, 0xAB, 0x11, 0x9A, 0x0A, 0x77, 0xCE, 0x5E, 0x0E,
    0xE8, 0xD1, 0x37, 0x72, 0xAC, 0x8C, 0x46, 0x03, 0xE5, 0x24, 0x09, 0x9E, 0x63, 0xB4, 0x2B, 0x01,
    0x74, 0xE3, 0x3D, 0xB7, 0xAB, 0x72, 0xFF, 0x88, 0x04, 0x96, 0xF1, 0x17, 0xDC, 0x55, 0x55, 0x77,
    0x10, 0xFA, 0x3C, 0x38, 0xEE, 0x97, 0x60, 0xA1, 0x12, 0xD6, 0x1E, 0xCA, 0x8E, 0x01, 0xD1, 0xA2,
    0x2F, 0x5B, 0xE2, 0xD6, 0x83, 0x9F, 0x26, 0xD9, 0x03, 0xCE, 0xE8, 0xE1, 0x98, 0xE3, 0xF0, 0x4C,
    0xE3, 0x0A, 0x7B, 0x05, 0x88, 0x1C, 0x63, 0x38, 0xB0, 0xAE, 0x94, 0xD1, 0xF2, 0x7F, 0x9C, 0x4B,
    0x55, 0x27, 0x6D, 0x34, 0x8F, 0x1A, 0x6D, 0x87, 0xD8, 0xBF, 0x2C, 0x15, 0xD1, 0xD7, 0x69, 0x37,
    0x19, 0xB2, 0xA1, 0x8D, 0xB4, 0xEE, 0xDB, 0x1F, 0xA7, 0xCE, 0xD2, 0x53, 0x14, 0x5E, 0x43, 0x90,
    0xDB, 0x39, 0x1A, 0xB9, 0xA8, 0x33, 0x02, 0x45, 0x24, 0x66, 0xED, 0xE0, 0xD9, 0x35, 0xC0, 0xA3,
    0x8A, 0x76, 0x98, 0x16, 0x38, 0x3D, 0x6D, 0x77, 0xC4, 0x5D, 0x91, 0x41, 0xEF, 0x8B, 0x5F, 0x62,
    0x58, 0xD5, 0xB7, 0xB0, 0x1E, 0x49, 0xC7, 0x3E, 0x8B, 0x05, 0xB4, 0x34, 0xBD, 0x49, 0xA1, 0x3E,
    0x04, 0x10, 0x3F, 0xC1, 0x52, 0x5B, 0xD3, 0x24, 0xDB, 0xEB, 0x4D, 0x5A, 0x16, 0x57, 0x79, 0x8B,
};


//----------------------------------------------------------------------------
// Get the CPU time in milliseconds in user space since the process started.
//----------------------------------------------------------------------------

uint64_t get_user_ms()
{
    ::rusage usage;
    if (::getrusage(RUSAGE_SELF, &usage) < 0) {
        perror("getrusage");
        ::exit(EXIT_FAILURE);
    }
    return uint64_t(usage.ru_utime.tv_sec) * 1000 + uint64_t(usage.ru_utime.tv_usec) / 1000;
}


//----------------------------------------------------------------------------
// Program entry point.
//----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    const int iterations = argc > 1 ? std::atoi(argv[1]) : DEFAULT_ITERATIONS;

    std::cout << "SHA-512 performance test, " << iterations << " iterations, " << sizeof(test_data) << " bytes" << std::endl;

    SHA512 sha;
    ArmSHA512 arm_sha;
    uint8_t hash[SHA512::HASH_SIZE];
    uint8_t arm_hash[SHA512::HASH_SIZE];

    bzero(hash, sizeof(hash));
    sha.init();
    uint64_t start = get_user_ms();
    for (int i = 0; i < iterations; ++i) {
        sha.add(test_data, sizeof(test_data));
    }
    const uint64_t time1 = get_user_ms() - start;
    sha.getHash(hash, sizeof(hash));

    std::cout << "Class SHA512:    time: " << time1 << " ms" << std::endl;

    bzero(arm_hash, sizeof(arm_hash));
    arm_sha.init();
    start = get_user_ms();
    for (int i = 0; i < iterations; ++i) {
        arm_sha.add(test_data, sizeof(test_data));
    }
    const uint64_t time2 = get_user_ms() - start;
    arm_sha.getHash(arm_hash, sizeof(arm_hash));
    const bool ok = ::memcmp(hash, arm_hash, sizeof(hash)) == 0;

    std::cout << "Class ArmSHA512: time: " << time2 << " ms, " << (ok ? "same hash" : "INVALID HASH") << std::endl;

    if (time2 > 0) {
        std::cout << "Performance ratio: " << (double(time1) / double(time2)) << std::endl;
    }

    return EXIT_SUCCESS;
}
