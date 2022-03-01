# MIA

Also known as Meta Information Adder, is designed to extend cpp capatibilities and provide missing features before(if ever) they are incorporated into standard and implemented into compilers.

## Current features

- `to_string` and `to_enum` overloads generation for enum conversion(outputs to `stdout`)

## Getting started

In order to compile Mia you need to install:

- Argumentum

- spdlog

- libclang

- clang

Argumentum and spdlog can be installed via `vcpkg`.

With vcpkg:

```bash
mkdir out
cd out
cmake ../ -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg/scripts/buildsystems/vcpkg.cmake> -DLLVM_CONFIG_BINARY=<path_to_llvm-config> -DLIBCLANG_INCLUDE_DIR=<path_to_llvm/clang/include>
```

## Planned featues

- proper files generation

- object serialisation

- reflection system

- proper cmake integration
