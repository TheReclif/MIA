#pragma once
#ifndef MIA_CPPASTUTILS_INCLUDED
#define MIA_CPPASTUTILS_INCLUDED

#include <string>

namespace cppast
{
	class cpp_entity;
	class cpp_type;
}

namespace mia::utils
{
	std::string getEntityParentFullName(const cppast::cpp_entity&);
	std::string getEntityFullyQualifiedName(const cppast::cpp_entity&);
	std::string getTypeName(const cppast::cpp_type&);
}

#endif
