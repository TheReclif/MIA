#pragma once
#ifndef MIA_ENUMCONVERSIONS_INCLUDED
#define MIA_ENUMCONVERSIONS_INCLUDED

#include <Generator.hpp>

namespace mia::modules
{
	class EnumConversionsModule : public GeneratorModule
	{
	public:
		virtual void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) override;
		virtual const char* getVersion() const override;
	};
}

#endif