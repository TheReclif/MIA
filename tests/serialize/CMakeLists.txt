add_executable(serialize Test.cpp)

target_link_libraries(serialize PRIVATE mia::utils)
target_include_directories(serialize PRIVATE .)
target_sources(serialize
PRIVATE
	SimpleTest.hpp
)

miagen(serialize)
