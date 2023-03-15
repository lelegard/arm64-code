# SHA-512 hash computation

This sample code compares the results and performances of SHA-512 computations.

The class `SHA512` is a standard portable implementation. The class `ArmSHA512`
uses the Arm64 SHA512 instructions.

On an Apple M1 processor (Apple Firestorm / Icestorm cores), on an Ubuntu 22.10
virtual machine with GCC 12.2, the specialized Arm64 implementation is XXXX times
faster than the portable implementation:
~~~
$ ./sha512_perf 10000000
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 cores), on an Ubuntu 22.04
virtual machine with GCC 11.3, the specialized Arm64 implementation is XXXX times
faster than the portable implementation:
~~~
$ ./sha512_perf 10000000
~~~
