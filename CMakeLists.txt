cmake_minimum_required(VERSION 3.14)

project(MIA VERSION 0.0.1 LANGUAGES CXX)

find_package(Argumentum CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(MIA MIA.CPP "include/App.hpp" "src/App.cpp")

add_subdirectory(cppast)

target_compile_features(MIA PRIVATE cxx_std_17)

target_link_libraries(MIA PRIVATE cppast Argumentum::argumentum spdlog::spdlog spdlog::spdlog_header_only)
target_include_directories(MIA PRIVATE include)

add_subdirectory(src)
