# SHA-256 hash computation

This sample code compares the results and performances of SHA-256 computations.

The class `SHA256` is a standard portable implementation. The class `ArmSHA256`
uses the Arm64 SHA256 instructions.

On an Apple M1 processor (Apple Firestorm / Icestorm cores), on an Ubuntu 22.10
virtual machine with GCC 12.2, the specialized Arm64 implementation is 9 times
faster than the portable implementation:
~~~
$ ./sha256_perf 10000000
SHA-256 performance test, 10000000 iterations, 256 bytes
Class SHA256:    time: 10728 ms
Class ArmSHA256: time: 1191 ms, same hash
Performance ratio: 9.00756
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 cores), on an Ubuntu 22.04
virtual machine with GCC 11.3, the specialized Arm64 implementation is 6.6 times
faster than the portable implementation:
~~~
$ ./sha256_perf 10000000
SHA-256 performance test, 10000000 iterations, 256 bytes
Class SHA256:    time: 14100 ms
Class ArmSHA256: time: 2111 ms, same hash
Performance ratio: 6.6793
~~~
