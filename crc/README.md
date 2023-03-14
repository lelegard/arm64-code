# CRC32 computation for MPEG2-TS sections

This sample code compares the results and performances of CRC32 computations
using the algorithm for MPEG2-TS sections, as defined in ISO/IEC 13818-1 and ITU H-222.0.

The class `CRC32` is the standard portable implementation as used in TSDuck.
The class `ArmCRC32` uses the Arm64 CRC32 instructions.

On an Apple M1 processor (Apple Firestorm / Icestorm cores), on an Ubuntu 22.10
virtual machine with GCC 12.2, the specialized Arm64 implementation is 20 times
faster than the portable implementation:
~~~
$ ./crc_perf 10000000
CRC32 performance test, 10000000 iterations, 256 bytes
Class CRC32:    crc: 0x45e770fc, time: 6532 ms
Class ArmCRC32: crc: 0x45e770fc, time: 322 ms
Performance ratio: 20.2857
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 cores), on an Ubuntu 22.04
virtual machine with GCC 11.3, the specialized Arm64 implementation is 30 times
faster than the portable implementation:
~~~
$ ./crc_perf 10000000
CRC32 performance test, 10000000 iterations, 256 bytes
Class CRC32:    crc: 0x45e770fc, time: 5938 ms
Class ArmCRC32: crc: 0x45e770fc, time: 201 ms
Performance ratio: 29.5423
~~~
