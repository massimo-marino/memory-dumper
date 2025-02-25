# memory-dumper
A memory dumper in C++

C++ implementation of a memory dumper.

Tested on Ubuntu 24.04.

See:

[https://web.archive.org/web/20210728162751/https://jrruethe.github.io/blog/2015/08/23/placement-new/](https://web.archive.org/web/20210728162751/https://jrruethe.github.io/blog/2015/08/23/placement-new/)

for original code.

A PDF copy of that web page is in folder `doc/`.


## Requirements

`cmake` is used to compile the sources.

The default compiler used is `clang++`.

The cmake files compile with `-std=c++20`.


## Install and Run the Example

```bash
$ git clone https://github.com:massimo-marino/memory-dumper.git
$ cd memory-dumper
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./mem-dump
```


## How to Use it

See the source code and the examples in `main.cpp`.
