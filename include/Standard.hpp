#pragma once
#ifndef MIA_STANDARD_INCLUDED
#define MIA_STANDARD_INCLUDED

namespace mia
{
	enum class [[mia_gen::enum_string]] CppStandard
	{
		Cpp98 [[mia_gen::enum_val_name("c++98")]],
		Cpp03 [[mia_gen::enum_val_name("c++03")]],
		Cpp11 [[mia_gen::enum_val_name("c++11")]],
		Cpp14 [[mia_gen::enum_val_name("c++14")]],
		Cpp1z [[mia_gen::enum_val_name("c++1z")]],
		Cpp17 [[mia_gen::enum_val_name("c++17")]],
		Cpp2a [[mia_gen::enum_val_name("c++2a")]],
		Cpp20 [[mia_gen::enum_val_name("c++20")]]
	};
}

#endif