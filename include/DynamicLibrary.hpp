#pragma once
#ifndef DYNAMICLIBRARY_INCLUDED
#define DYNAMICLIBRARY_INCLUDED

#include <core_export.h>

#include <string_view>
#include <memory>

namespace mia
{
	struct CORE_EXPORT DynamicLibrary
	{
		using DummyFuncPtr = void(*)();

		virtual ~DynamicLibrary() = default;

		virtual bool load(const char* name) = 0;
		virtual DummyFuncPtr getFuncAddress(const char* name) const = 0;
		virtual std::string_view getName() const = 0;
	};

	struct CORE_EXPORT DynamicLibraryFactory
	{
		static std::unique_ptr<DynamicLibrary> create();
	};
}

#endif
