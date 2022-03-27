target_sources(utils PRIVATE
	MiaUtils.hpp
)

install(FILES MiaUtils.hpp DESTINATION include)

target_sources(mia-core PRIVATE
	StringUtils.hpp
	Standard.hpp
	CodeGenerator.hpp
	Generator.hpp
	CppastUtils.hpp
	Type.hpp
)

target_sources(mia PRIVATE
	App.hpp
)

add_subdirectory(modules)