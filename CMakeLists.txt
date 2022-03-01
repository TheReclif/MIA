cmake_minimum_required(VERSION 3.14)

project(MIA VERSION 0.0.1 LANGUAGES CXX)

find_package(Argumentum CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(cppast)

add_library(utils INTERFACE)
target_sources(utils INTERFACE include/MiaUtils.hpp)
target_include_directories(utils INTERFACE include)

add_library(mia-core)

target_compile_features(mia-core PRIVATE cxx_std_17)
target_link_libraries(mia-core PUBLIC utils cppast spdlog::spdlog spdlog::spdlog_header_only)
target_include_directories(mia-core PRIVATE include)

add_executable(mia Mia.cpp)

target_compile_features(mia PRIVATE cxx_std_17)
target_link_libraries(mia PRIVATE mia-core Argumentum::argumentum)
target_include_directories(mia PRIVATE include)

add_subdirectory(src)
add_subdirectory(include)

set(mia_executable_path "$<TARGET_FILE:mia>")

configure_file(
	"${PROJECT_SOURCE_DIR}/cmake/mia.cmake.in"
    "${PROJECT_BINARY_DIR}/cmake/mia.cmake"
	@ONLY
)