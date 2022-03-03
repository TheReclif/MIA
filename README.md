# MIA

Also known as Meta Information Adder, is designed to extend cpp capatibilities and provide missing features before(if ever) they are incorporated into standard and implemented into compilers.

## Current features

- `to_string` and `to_enum` overloads generation for enum conversion(outputs to `stdout`)
- basic cmake integration

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
cmake --build . --target install
```

### Basic usage

#### Help

To get most up to date help, type:

```bash
./mia -h
```

It will show all flags, commands and their options with descriptions.

#### Cmake integration

When installed, mia is available as cmake package, so you can import it from your cmake script:

```cmake
find_package(mia REQUIRED CONFIG)
```

Then, you can use `miagen` function that enables metainformation generation for given target. Right now it adds subtarget for every header in project. In the future, there will be alternative that does not add any target, but generates information in batch for every build instead of per header modification.

It accepts target as only argument:

```cmake
miagen(myTarget)
```

#### Manual usage

Most basic way to generate headers is to simply specify input and output file during mia invokation:

```bash
./mia -f myHeader.hpp -o myHeader.mia.hpp
```

Additionally, you can specify cpp standard version, default is `c++11`:

```bash
./mia --std=c++17 -f [...]
```

## Advanced usage

### Cmake

If built-in cmake intergration does not suit your needs, mia exposes `mia_executable_path` that stores path to installed mia executable.

### Multiple files

Generating headers one by one is not the most efficient and ellegant way of using mia.

When dealing wih multiple files, you should specify output pattern, so that mia knows how to create output file name from input files:

```bash
./mia -f h1.hpp h2.hpp h3.hpp -o "generated/{}.hpp"
```

Basically, `{}`  or `{0}`(see `fmt` documentation for more info) gets replaced with input file name.

### Performance

By default, mia uses all available threads to generate output(every file can be processed by at most one thread at once). If you want to limit thread usage, you can specify maximum number of threads:

```bash
./mia -t 2 -f [...]
```

### Debugging

If you need to check if meta-data can be generated without actually generating output files, you can run mia in dry mode with `--dry-run` or `-d` flag.

Additionally to not outputing files, it will print definitions to stdout.

Currently, it exits with code `0` even if mia fails for some files. This will change in the future(mia will return `1` if even it fails only for one file).

## Planned featues

- object serialisation

- reflection system

- proper cmake integration
