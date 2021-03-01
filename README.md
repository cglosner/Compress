# Compress

## Overview

This repository has two c++ files and three different sized 
`.txt` files. The two c++ files are `multi_thread_compress.cpp`
and `compress.cpp`, which are very straight forward.
`multi_thread_compress.cpp` compresses a file with a configurable
number of threads, and `compress.cpp` only compresses an
input file on the main thread. The main idea of this
repository is to compare the compression time between the
two methods.

## Test System

The code was tested on a computer running Windows 10
with an a i7-5960 CPU @ 3.00GHz with 64GBs of RAM.
Windows Linux Subsystem was used to run the c++ code.

## Input File Descriptions

There are three input files: `Large_Input_File.txt`,
`Medium_Input_File.txt`, and `Small_Input_File.txt`.
The `Large_Input_File.txt` is roughly 1GB and tests
how well the compression works on large files. The
`Medium_Input_File.txt` is roughly 20MBs and is designed
to give comparitive results between the smaller and
larger files. The `Small_Input_File.txt` is 779KBs.
The `Small_Input_File.txt` is a book that was obtained
from the opensource library called [The Gutenberg Project](https://www.gutenberg.org/),
which contains books in the public domain availiable to 
download in a variety of forms.

## C++ File Descriptions

The purpose of the two descriptions below is to give
a general overview of what the two c++ files do, and
give further explaination on specific functions or
lines of code that require it.

Note, it requires the use of the `zlib` and `pthread`
libraries. The `pthread` library should automatically
be downloaded, but `zlib` might not be if you are
using the Windows Linux Subsystem. The `zlib` library
can be obtained with:

```
sudo apt-get install zlib1g-dev
```

For compiling the code, the following line was used:

```
g++ multi_thread_compress.cpp -lz -g -pthread -o multi.out
```

That exact line isn't required, but if you choose to 
use a different compiling line the `-lz` is needed for
the `zlib` library and the `-pthread` is needed for the 
pthread library. Those lines may be different for different
compilers.

Finally, for both of the files below decompression is possible, 
but it requires the size of the individual compressed blocks
so it can decompress properly. To do that a second output file
is written called `Decompress_Blocks.txt`, which includes the
compressed block sizes because otherwise it wouldn't be able to
decompress correctly.

### `multi_thread_compress.cpp`



### `compress.cpp`



## Analysis

### Description

There are three important comparison methods used
to exaluate the multithreading used for compression:
how fast is took to compress, how the number of threads effects the
speed, and finally, the size of the compression. 
Note, when worker threads are described below
that is reffering to the number of threads that are
used to compress the input file on excluding the main thread,
so when it says 0 worker threads that means only the main 
thread was used (i.e. `compress.cpp`).

### Comparison

| Worker Threads | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Size | | | | | |
| 779KB |
| 20MB |
| 1GB |

## Conclusion


