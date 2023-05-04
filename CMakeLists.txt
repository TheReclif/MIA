cmake_minimum_required(VERSION 3.14)

project(MIA VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)
include(GenerateExportHeader)

find_package(Argumentum CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_package(Git REQUIRED)

execute_process(COMMAND "${GIT_EXECUTABLE}" apply ../fix_cppast_install.patch WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cppast" ERROR_QUIET)

message(STATUS "Fetching debug_assert")
set(DEBUG_ASSERT_INSTALL ON CACHE BOOL "[reserved] forcing debug_assert to install")
FetchContent_Declare(debug_assert GIT_REPOSITORY https://github.com/foonathan/debug_assert.git GIT_TAG origin/master)
FetchContent_MakeAvailable(debug_assert)

add_subdirectory(cppast)

add_library(utils INTERFACE)
target_sources(utils INTERFACE)
target_include_directories(utils INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

add_library(core SHARED)

generate_export_header(core)
target_compile_features(core PRIVATE cxx_std_17)
target_link_libraries(core PUBLIC utils cppast)
target_link_libraries(core PRIVATE Argumentum::argumentum spdlog::spdlog spdlog::spdlog_header_only)
target_include_directories(core PRIVATE include PUBLIC
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)
target_compile_definitions(core PUBLIC MIA_VERSION="${CMAKE_PROJECT_VERSION}")

add_executable(mia Mia.cpp)

target_compile_features(mia PRIVATE cxx_std_17)
target_link_libraries(mia PRIVATE core Argumentum::argumentum spdlog::spdlog spdlog::spdlog_header_only)
target_include_directories(mia PRIVATE include)

add_subdirectory(src)
add_subdirectory(include)

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
install(TARGETS cppast type_safe _cppast_tiny_process _cppast_libclang tiny-process-library EXPORT miaPreqs LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
install(TARGETS utils core EXPORT miaTargets LIBRARY DESTINATION bin PUBLIC_HEADER DESTINATION include RUNTIME DESTINATION bin ARCHIVE DESTINATION lib)
install(EXPORT miaPreqs DESTINATION lib/cmake/mia NAMESPACE mia::)
install(EXPORT miaTargets DESTINATION lib/cmake/mia NAMESPACE mia::)
install(FILES "${PROJECT_BINARY_DIR}/core_export.h" DESTINATION include)
install(FILES 
    "${PROJECT_BINARY_DIR}/miaConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/miaConfig.cmake"
    DESTINATION lib/cmake/mia
)