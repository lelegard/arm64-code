# SHA-512 hash computation

This sample code compares the results and performances of SHA-512 computations.

The class `SHA512` is a standard portable implementation. The class `ArmSHA512`
uses the Arm64 SHA512 instructions.

On an Apple M1 processor (Apple Firestorm / Icestorm cores), on an Ubuntu 22.10
virtual machine with GCC 12.2, the specialized Arm64 implementation is 3 times
faster than the portable implementation:
~~~
$ ./sha512_perf 10000000
SHA-512 performance test, 10000000 iterations, 256 bytes
Class SHA512:    time: 5539 ms
Class ArmSHA512: time: 1849 ms, same hash
Performance ratio: 2.99567
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 cores), on an Ubuntu 22.04
virtual machine with GCC 11.3, the specialized Arm64 implementation is 3.5 times
faster than the portable implementation:
~~~
$ ./sha512_perf 10000000
SHA-512 performance test, 10000000 iterations, 256 bytes
Class SHA512:    time: 9154 ms
Class ArmSHA512: time: 2727 ms, same hash
Performance ratio: 3.3568
~~~

On a nVidia Grace processor (Arm Neoverse V2 cores), on RHEL 9.5 with GCC 11.5,
the specialized Arm64 implementation is 3.2 times faster than the portable implementation:
~~~
$ ./sha512_perf 10000000
SHA-512 performance test, 10000000 iterations, 256 bytes
Class SHA512:    time: 6984 ms
Class ArmSHA512: time: 2165 ms, same hash
Performance ratio: 3.22587
~~~
