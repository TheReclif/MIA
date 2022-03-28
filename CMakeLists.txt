cmake_minimum_required(VERSION 3.14)

project(MIA VERSION 0.0.1 LANGUAGES CXX)

find_package(Argumentum CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(cppast)

add_library(utils INTERFACE)
target_sources(utils INTERFACE)
target_include_directories(utils INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

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

# TESTS

add_executable(test Test.cpp)
target_compile_features(test PUBLIC cxx_std_17)
target_link_libraries(test PUBLIC mia-core)
target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# INSTALLS

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/miaConfigVersion.cmake"
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/miaConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/miaConfig.cmake"
    INSTALL_DESTINATION lib/cmake/mia
)

install(TARGETS mia DESTINATION bin)
install(TARGETS utils EXPORT miaTargets LIBRARY DESTINATION lib)
install(EXPORT miaTargets DESTINATION lib/cmake/mia NAMESPACE mia::)
install(FILES 
    "${PROJECT_BINARY_DIR}/miaConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/miaConfig.cmake"
    DESTINATION lib/cmake/mia
)