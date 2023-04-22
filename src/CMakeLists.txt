target_sources(mia-core PRIVATE
	DynamicLibrary.cpp
	StringUtils.cpp
	CodeGenerator.cpp
	Generator.cpp
	CppastUtils.cpp
)

target_sources(mia PRIVATE
	App.cpp
)

add_subdirectory(modules)