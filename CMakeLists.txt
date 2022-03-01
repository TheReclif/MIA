cmake_minimum_required(VERSION 3.14)

project(MIA VERSION 0.0.1 LANGUAGES CXX)

find_package(Argumentum CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


add_library(utils INTERFACE)
target_sources(utils INTERFACE include/MiaUtils.hpp)
target_include_directories(utils INTERFACE include)

add_executable(mia Mia.cpp)

add_subdirectory(cppast)

target_compile_features(mia PRIVATE cxx_std_17)

target_link_libraries(mia PRIVATE utils cppast Argumentum::argumentum spdlog::spdlog spdlog::spdlog_header_only)
target_include_directories(mia PRIVATE include)

add_subdirectory(src)
add_subdirectory(include)
