cmake_minimum_required(VERSION 3.13)

project(test LANGUAGES CXX)

find_package(mia REQUIRED CONFIG)

add_executable(test main.cpp)

target_link_libraries(test PRIVATE mia::utils)
target_include_directories(test PRIVATE .)
target_sources(test
PRIVATE
	defs.hpp
)

miagen(test)