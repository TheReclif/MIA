#pragma once
#ifndef MIA_STANDARD_INCLUDED
#define MIA_STANDARD_INCLUDED

namespace mia
{
	enum class [[mia::include]] CppStandard
	{
		Cpp98 [[mia::name("c++98")]],
		Cpp03 [[mia::name("c++03")]],
		Cpp11 [[mia::name("c++11")]],
		Cpp14 [[mia::name("c++14")]],
		Cpp1z [[mia::name("c++1z")]],
		Cpp17 [[mia::name("c++17")]],
		Cpp2a [[mia::name("c++2a")]],
		Cpp20 [[mia::name("c++20")]]
	};
}

#endif