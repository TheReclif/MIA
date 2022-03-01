cmake_minimum_required(VERSION 3.13)

project(test LANGUAGES CXX)

include(../../out/cmake/mia.cmake)

add_executable(test main.cpp)

target_include_directories(test PRIVATE .)
target_sources(test
PRIVATE
	defs.hpp
)

miagen("test")