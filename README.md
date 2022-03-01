# MIA

Also known as Meta Information Adder, is designed to extend cpp capatibilities and provide missing features before(if ever) they are incorporated into standard and implemented into compilers.

## Current features

- `to_string` and `to_enum` overloads generation for enum conversion(outputs to `stdout`)

## Getting started

### Prerequisites

In order to compile Mia you need to install:

- Argumentum

- spdlog

- libclang

- clang

Argumentum and spdlog can be installed via `vcpkg`.

### Building

With vcpkg:

```bash
mkdir out
cd out
cmake ../ -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg/scripts/buildsystems/vcpkg.cmake> -DLLVM_CONFIG_BINARY=<path_to_llvm-config> -DLIBCLANG_INCLUDE_DIR=<path_to_llvm/clang/include>
cmake --build .
```

### Basic usage

To get most up to date help, type:

```bash
./mia
```

File generation is currently not supported yet, but there is a temporary solution.

Run:

```bash
./mia -f <files> -d
```

It will output source code of generated functions. Copy it to some header file, and add this 2 lines at the beginning of the header:

```cpp
#pragma once
#include <MiaUtils.hpp>
```



## Planned featues

- proper files generation

- object serialisation

- reflection system

- proper cmake integration
