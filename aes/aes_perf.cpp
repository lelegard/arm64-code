//----------------------------------------------------------------------------
//
// Arm64 CPU system registers tools
// Copyright (c) 2023, Thierry Lelegard
// BSD-2-Clause license, see the LICENSE file.
//
// Comparative performance test on AES (portable vs. Arm64 instructions).
// Specify the number of iterations on the command line.
//
//----------------------------------------------------------------------------

#include "AES.h"
#include "ArmAES.h"
#include <ios>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <sys/resource.h>

#define DEFAULT_ITERATIONS 10000000

struct TestData {
    size_t  key_size;
    uint8_t key[32];
    uint8_t plain[16];
    uint8_t cipher[16];
};

// Only one test vector per key size.
static const TestData test_data[] = {
    {
        16,
        {0xEC, 0xC4, 0xBD, 0x51, 0x4D, 0x54, 0x53, 0xA9,
         0x02, 0x9A, 0xE3, 0xD2, 0xEA, 0x4A, 0x9E, 0xF4},
        {0x33, 0xDB, 0xF0, 0x87, 0x79, 0xC4, 0xE7, 0x4B,
         0x3E, 0x96, 0x1C, 0x7A, 0x0C, 0x4B, 0x33, 0x0B},
        {0x20, 0x3B, 0x27, 0x41, 0xC9, 0x3D, 0x1B, 0x43,
         0x7C, 0xA9, 0xBC, 0xF9, 0x51, 0xC6, 0xC9, 0xA5},
    },
    {
        24,
        {0xB7, 0x66, 0xE6, 0xFA, 0xD9, 0x5D, 0xC3, 0x7E,
         0xBB, 0x16, 0xE3, 0xB0, 0x47, 0x07, 0xAB, 0xE3,
         0xD6, 0x97, 0xC3, 0xAC, 0xB6, 0xA4, 0x47, 0x9F},
        {0xEC, 0xF6, 0x24, 0x8B, 0x58, 0x34, 0x75, 0xF5,
         0x70, 0xFF, 0x38, 0x69, 0x07, 0x01, 0x63, 0x6D},
        {0x13, 0x84, 0x77, 0x7D, 0x26, 0x84, 0xC9, 0x5A,
         0xC4, 0x79, 0x33, 0xC1, 0xD4, 0x30, 0x9F, 0x9D},
    },
    {
        32,
        {0x0D, 0x43, 0x48, 0xF3, 0x57, 0xA3, 0x6A, 0xA9,
         0x1E, 0x94, 0x78, 0xA9, 0xCD, 0xA8, 0x87, 0x00,
         0xBA, 0xB7, 0xC7, 0x0B, 0xBA, 0x3B, 0xB2, 0xC8,
         0x21, 0x0C, 0xFC, 0x08, 0x0B, 0x3B, 0x2C, 0x95},
        {0x6F, 0x4F, 0x57, 0x07, 0xDB, 0x41, 0x56, 0x75,
         0xB4, 0xFF, 0xED, 0x33, 0x85, 0xCD, 0x51, 0x91},
        {0xFE, 0x33, 0xC3, 0x3D, 0x8E, 0xE5, 0xEB, 0xD4,
         0xCB, 0x16, 0xBF, 0x00, 0x6F, 0x8E, 0x56, 0x1A},
    },
    {0, {}, {}}
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

    std::cout << std::endl << "AES performance test, " << iterations << " iterations per operation " << std::endl << std::endl;

    AES aes;
    ArmAES arm_aes;
    uint8_t output[16];

    for (auto test = test_data; test->key_size > 0; ++test) {

        aes.setKey(test->key, test->key_size);
        arm_aes.setKey(test->key, test->key_size);

        uint64_t start = get_user_ms();
        for (int i = 0; i < iterations; ++i) {
            aes.encrypt(test->plain, sizeof(test->plain), output, sizeof(output), nullptr);
        }
        const uint64_t time1e = get_user_ms() - start;

        if (::memcmp(output, test->cipher, sizeof(output)) == 0) {
            std::cout << "Class SHA1:    AES-" << (test->key_size * 8) << " encrypt, time: " << time1e << " ms" << std::endl;
        }
        else {
            std::cout << "Class SHA1:    AES-" << (test->key_size * 8) << " encrypt FAILED" << std::endl;
        }

        start = get_user_ms();
        for (int i = 0; i < iterations; ++i) {
            aes.decrypt(test->cipher, sizeof(test->cipher), output, sizeof(output), nullptr);
        }
        const uint64_t time1d = get_user_ms() - start;

        if (::memcmp(output, test->plain, sizeof(output)) == 0) {
            std::cout << "Class SHA1:    AES-" << (test->key_size * 8) << " decrypt, time: " << time1d << " ms" << std::endl;
        }
        else {
            std::cout << "Class SHA1:    AES-" << (test->key_size * 8) << " decrypt FAILED" << std::endl;
        }

        start = get_user_ms();
        for (int i = 0; i < iterations; ++i) {
            arm_aes.encrypt(test->plain, sizeof(test->plain), output, sizeof(output), nullptr);
        }
        const uint64_t time2e = get_user_ms() - start;

        if (::memcmp(output, test->cipher, sizeof(output)) == 0) {
            std::cout << "Class ArmSHA1: AES-" << (test->key_size * 8) << " encrypt, time: " << time2e << " ms" << std::endl;
        }
        else {
            std::cout << "Class ArmSHA1: AES-" << (test->key_size * 8) << " encrypt FAILED" << std::endl;
        }

        start = get_user_ms();
        for (int i = 0; i < iterations; ++i) {
            arm_aes.decrypt(test->cipher, sizeof(test->cipher), output, sizeof(output), nullptr);
        }
        const uint64_t time2d = get_user_ms() - start;

        if (::memcmp(output, test->plain, sizeof(output)) == 0) {
            std::cout << "Class ArmSHA1: AES-" << (test->key_size * 8) << " decrypt, time: " << time2d << " ms" << std::endl;
        }
        else {
            std::cout << "Class ArmSHA1: AES-" << (test->key_size * 8) << " decrypt FAILED" << std::endl;
        }

        std::cout << "Performance ratio: encrypt: " << (time2e > 0 ? double(time1e) / double(time2e) : 0.0)
                  << ", decrypt: " << (time2d > 0 ? double(time1d) / double(time2d) : 0.0)
                  << std::endl << std::endl;
    }

    return EXIT_SUCCESS;
}
