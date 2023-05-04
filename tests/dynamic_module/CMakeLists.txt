add_library(dynamic_module SHARED "MiaTestModule.cpp")

target_link_libraries(dynamic_module PRIVATE mia::utils mia::core)
