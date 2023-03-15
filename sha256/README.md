# SHA-256 hash computation

This sample code compares the results and performances of SHA-256 computations.

The class `SHA256` is a standard portable implementation. The class `ArmSHA256`
uses the Arm64 SHA256 instructions.

On an Apple M1 processor (Apple Firestorm / Icestorm cores), on an Ubuntu 22.10
virtual machine with clang 15.0.6, the specialized Arm64 implementation is 10 times
faster than the portable implementation:
~~~
$ ./sha256_perf 10000000
SHA-256 performance test, 10000000 iterations, 256 bytes
Class SHA256:    time: 13004 ms
Class ArmSHA256: time: 1255 ms, same hash
Performance ratio: 10.3618
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 cores), on an Ubuntu 22.04
virtual machine with clang 14.0.0, the specialized Arm64 implementation is 6.5 times
faster than the portable implementation:
~~~
$ ./sha256_perf 10000000
SHA-256 performance test, 10000000 iterations, 256 bytes
Class SHA256:    time: 13802 ms
Class ArmSHA256: time: 2111 ms, same hash
Performance ratio: 6.53813
~~~

Note: We used clang instead of gcc since it is currently impossible to compile SHA256 intrinsics with gcc. See
https://stackoverflow.com/questions/75745202/arm64-sha512-intrinsics-fail-to-compile-with-gcc-target-specific-option-mismat
