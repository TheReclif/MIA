#pragma once
#ifndef MIA_CPPASTUTILS_INCLUDED
#define MIA_CPPASTUTILS_INCLUDED

#include <string>

namespace cppast
{
	class cpp_entity;
}

namespace mia::utils
{
	std::string getEntityParentFullName(const cppast::cpp_entity&);
}

#endif
