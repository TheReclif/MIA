#pragma once
#ifndef MIA_ENUMCONVERSIONS_INCLUDED
#define MIA_ENUMCONVERSIONS_INCLUDED

#include <core_export.h>
#include <Generator.hpp>

namespace mia::modules
{
	class CORE_EXPORT EnumConversionsModule : public GeneratorModule
	{
	public:
		virtual void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) override;
		virtual const char* getVersion() const override;
	};
}

#endif