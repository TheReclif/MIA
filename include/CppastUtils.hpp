#pragma once
#ifndef MIA_CPPASTUTILS_INCLUDED
#define MIA_CPPASTUTILS_INCLUDED

#include <string>

namespace cppast
{
	class cpp_entity;
	class cpp_type;
	class cpp_class_template;
}

namespace mia::utils
{
	std::string getEntityParentFullName(const cppast::cpp_entity&);
	std::string getEntityFullyQualifiedName(const cppast::cpp_entity&);
	std::string getTypeName(const cppast::cpp_type&);
	const cppast::cpp_class_template* isTemplate(const cppast::cpp_entity&);
}

#endif
