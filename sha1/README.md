# SHA-1 hash computation

This sample code compares the results and performances of SHA-1 computations.

The class `SHA1` is a standard portable implementation. The class `ArmSHA1`
uses the Arm64 SHA1 instructions.

On an Apple M1 processor, the specialized Arm64 implementation is 7.7 times faster
than the portable implementation:
~~~
$ ./sha1_perf 10000000
SHA-1 performance test, 10000000 iterations, 256 bytes
Class SHA1:    time: 9029 ms
Class ArmSHA1: time: 1167 ms, same hash
Performance ratio: 7.73693
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 core), the specialized Arm64
implementation is 4.6 times faster than the portable implementation:
~~~
$ ./sha1_perf 10000000
SHA-1 performance test, 10000000 iterations, 256 bytes
Class SHA1:    time: 8100 ms
Class ArmSHA1: time: 1741 ms, same hash
Performance ratio: 4.6525
~~~
