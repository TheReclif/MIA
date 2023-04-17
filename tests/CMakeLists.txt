cmake_minimum_required(VERSION 3.13)

project(test LANGUAGES CXX)

find_package(mia REQUIRED CONFIG)

add_subdirectory(serialize)