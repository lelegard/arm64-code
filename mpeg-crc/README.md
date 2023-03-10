# CRC32 computation for MPEG2-TS sections

This sample code compares the results and performances of CRC32 computations
using the algorithm for MPEG2-TS sections, as defined in ISO/IEC 13818-1 and ITU H-222.0.

The class `CRC32` is the standard portable implementation as used in TSDuck.

The class `ArmCRC32` uses the Arm64 CRC32 instructions and is 20 times faster
than the portable implementation:
~~~
$ ./crc_perf 10000000
CRC32 performance test, 10000000 iterations, 256 bytes
Class CRC32:    crc: 0x45e770fc, time: 6448 ms
Class ArmCRC32: crc: 0x45e770fc, time: 317 ms
Performance ratio: 20.3407
~~~
