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

### Multithread Compression: `multi_thread_compress.cpp`

This will need a little more explaination than the single
thread compression code because it is more complicated, but
first, executing the compiled code has the following form:

```
./run.out input1 input2 input3 input4
```

where `input1` is either `-c` or `-d` for compress or decompress, `input2`
is for the number of worker threads to use, `input3` is the input file,
and `input4` is the output file. An example is below:

```
./multi.out -c 4 Small_Input_File.txt Small_Multi_Compression.txt
```

Now to explain the program, there is a function that reads in a file and stores the
data in a vector in 4KB blocks, a function that runs the compression portion
of the code, and finally, a function that writes the output. Only the compression
function will need to be described because the other two are very simple. 
The compression function loops over all of the blocks in the vector to compress
them, but there is an inner loop used for creating the configurable number of
threads. The data is passed into the standard compression function through
the thread creation and execution, and then the thread is "joined" back
to the main thread in order of execution where the returned data is
pushed onto an output vector of blocks. 

### Single Thread Compression: `compress.cpp`

This code is straight forward and has comments throughout to help with
descriptions. The main thing to note is when running the compiled file
it follows the form:

```
./run.out input1 input2 input3
```

where `input1` is either `-c` or `-d` for compress or decompress, `input2` is the input file,
and `input3` is the output file. An example is below:

```
./compress.out -c Small_Input_File.txt Small_Compress.txt
```

The program, in general, retrieves input blocks of size 4KB, 
then compresses it, and then writes it to the output. The compression
code is very simple and is from the `zlib` library. 


## Analysis

### Description

There are two important comparison methods used
to exaluate the multithreading used for compression:
how fast is took to compress and the number of threads effects the
speed. Note, when worker threads are described below
that is reffering to the number of threads that are
used to compress the input file on excluding the main thread,
so when it says 0 worker threads that means only the main 
thread was used (i.e. `compress.cpp`).

### Comparison

| Worker Threads | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Size | | | | | |
| 779KB | 0.079 | 0.181 | 0.07 | 0.054 | 0.061 | 0.063 | 0.069 |
| 20MB | 1.578 | 4.814 | 2.000 | | 1.311 | 1.44 | |
| 1GB | 80.85 | 138.22 | 69.033 | 42.018 | 26.266 | 29.8 | 30.72 |

## Conclusion


