cmake_minimum_required(VERSION 3.13)

project(test LANGUAGES CXX)

find_package(mia REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(serialize)
add_subdirectory(dynamic_module)
