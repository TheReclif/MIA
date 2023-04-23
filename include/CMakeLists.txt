target_sources(utils PRIVATE
	MiaUtils.hpp
)

install(FILES MiaUtils.hpp Type.hpp AttributableBase.hpp DESTINATION include)

target_sources(core PRIVATE
	StringUtils.hpp
	Standard.hpp
	CodeGenerator.hpp
	Generator.hpp
	CppastUtils.hpp
	Type.hpp
	AttributableBase.hpp
)

target_sources(mia PRIVATE
	App.hpp
)

add_subdirectory(modules)