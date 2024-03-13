set(utils_headers
	${CMAKE_CURRENT_SOURCE_DIR}/MiaUtils.hpp
)

target_sources(utils PRIVATE
	${utils_headers}
)
set_target_properties(utils PROPERTIES PUBLIC_HEADER "${utils_headers}")

set(core_headers
	${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Standard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Standard_Gen.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/CodeGenerator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Generator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/CppastUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Type.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/AttributableBase.hpp
)

target_sources(core PRIVATE
	${core_headers}
)

set_target_properties(core PROPERTIES PUBLIC_HEADER "${core_headers}")

target_sources(mia PRIVATE
	App.hpp
)

add_subdirectory(modules)