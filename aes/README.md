# AES encryption and decryption

This sample code compares the results and performances of AES encryptions and decryptions.

The class `AES` is a standard portable implementation. The class `ArmAES`
uses the Arm64 AES instructions.

On an Apple M1 processor (Apple Firestorm / Icestorm cores), on an Ubuntu 22.10
virtual machine with GCC 12.2, the specialized Arm64 implementation is 17 to 20
times faster than the portable implementation:
~~~
$ ./aes_perf 100000000

AES performance test, 100000000 iterations per operation 

Class AES:    AES-128 encrypt, time: 3780 ms
Class AES:    AES-128 decrypt, time: 4067 ms
Class ArmAES: AES-128 encrypt, time: 219 ms
Class ArmAES: AES-128 decrypt, time: 219 ms
Performance ratio: encrypt: 17.2603, decrypt: 18.5708

Class AES:    AES-192 encrypt, time: 4597 ms
Class AES:    AES-192 decrypt, time: 4908 ms
Class ArmAES: AES-192 encrypt, time: 250 ms
Class ArmAES: AES-192 decrypt, time: 250 ms
Performance ratio: encrypt: 18.388, decrypt: 19.632

Class AES:    AES-256 encrypt, time: 5384 ms
Class AES:    AES-256 decrypt, time: 5774 ms
Class ArmAES: AES-256 encrypt, time: 281 ms
Class ArmAES: AES-256 decrypt, time: 282 ms
Performance ratio: encrypt: 19.1601, decrypt: 20.4752
~~~

On an AWS Graviton 3 processor (Arm Neoverse V1 cores), on an Ubuntu 22.04
virtual machine with GCC 11.3, the specialized Arm64 implementation is 18 to 20
times faster than the portable implementation:
~~~
$ ./aes_perf 100000000

AES performance test, 100000000 iterations per operation

Class AES:    AES-128 encrypt, time: 5662 ms
Class AES:    AES-128 decrypt, time: 5744 ms
Class ArmAES: AES-128 encrypt, time: 279 ms
Class ArmAES: AES-128 decrypt, time: 276 ms
Performance ratio: encrypt: 20.2939, decrypt: 20.8116

Class AES:    AES-192 encrypt, time: 6779 ms
Class AES:    AES-192 decrypt, time: 6760 ms
Class ArmAES: AES-192 encrypt, time: 333 ms
Class ArmAES: AES-192 decrypt, time: 342 ms
Performance ratio: encrypt: 20.3574, decrypt: 19.7661

Class AES:    AES-256 encrypt, time: 7792 ms
Class AES:    AES-256 decrypt, time: 7840 ms
Class ArmAES: AES-256 encrypt, time: 439 ms
Class ArmAES: AES-256 decrypt, time: 433 ms
Performance ratio: encrypt: 17.7494, decrypt: 18.1062
~~~

In all cases, using the accelerated AES instructions, the encryption and
decryption time are similar or even identical. Using the portable code,
the decryption is slightly slower than encryption, especially on the Apple M1,
less significantly on the Graviton 3.

On the Apple M1, the performance gain of the accelerated AES instructions
is higher with longer keys while this is the opposite on the Graviton 3.
