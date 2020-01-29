# C++ Wrapper for ALSA C API #

[![CircleCI](https://circleci.com/gh/maximus5684/alsaplusplus.svg?style=svg)](https://circleci.com/gh/maximus5684/alsaplusplus)

A basic C++14 wrapper for the ALSA C API on Linux.

## Requirements:
* A Linux-based system
* CMake 3.1 or higher
* The ALSA C API (libasound2-dev on Ubuntu/Debian)

## Installation:
1. Clone this repository.
2. In the cloned folder, run:

```
mkdir build && cd build
cmake ..
make
sudo make install
```

If you want the example files to be built/installed, change the `cmake` command above to:

```
cmake .. -DWITH_EXAMPLES=ON
```

## Usage:
See the example files in the repository or the header files.
