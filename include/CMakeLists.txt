target_sources(mia-core PRIVATE
	StringUtils.hpp
	Standard.hpp
	CodeGenerator.hpp
	Generator.hpp
)

target_sources(mia PRIVATE
	App.hpp
)

add_subdirectory(modules)