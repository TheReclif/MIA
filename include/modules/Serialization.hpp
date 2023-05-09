#pragma once
#ifndef MIA_SERIALIZATION_INCLUDED
#define MIA_SERIALIZATION_INCLUDED

#include <core_export.h>
#include <Generator.hpp>

namespace mia::modules
{
	class CORE_EXPORT SerializationModule : public GeneratorModule
	{
	public:
		virtual void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) override;
	};
}

#endif
