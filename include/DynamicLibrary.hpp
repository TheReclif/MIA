#pragma once
#ifndef DYNAMICLIBRARY_INCLUDED
#define DYNAMICLIBRARY_INCLUDED

#include <string_view>
#include <memory>

namespace mia
{
	struct DynamicLibrary
	{
		using DummyFuncPtr = void(*)();

		virtual ~DynamicLibrary() = default;

		virtual void load(const char* name) = 0;
		virtual DummyFuncPtr getFuncAddress(const char* name) const = 0;
	};

	struct DynamicLibraryFactory
	{
		static std::unique_ptr<DynamicLibrary> create();
	};
}

#endif
