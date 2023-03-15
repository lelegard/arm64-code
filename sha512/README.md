# SHA-512 hash computation

This sample code compares the results and performances of SHA-512 computations.

The class `SHA512` is a standard portable implementation. The class `ArmSHA512`
uses the Arm64 SHA512 instructions.

On an Apple M1 processor (Apple Firestorm / Icestorm cores), on an Ubuntu 22.10
virtual machine with clang 15.0.6, the specialized Arm64 implementation is 4 times
faster than the portable implementation:
~~~
$ ./sha512_perf 10000000
SHA-512 performance test, 10000000 iterations, 256 bytes
Class SHA512:    time: 7929 ms
Class ArmSHA512: time: 1910 ms, same hash
Performance ratio: 4.15131
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 cores), on an Ubuntu 22.04
virtual machine with clang 14.0.0, the specialized Arm64 implementation is XXXX times
faster than the portable implementation:
~~~
$ ./sha512_perf 10000000
~~~
