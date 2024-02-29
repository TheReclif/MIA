
#pragma once
#ifndef STANDARD_INCLUDED
#define STANDARD_INCLUDED

#include <MiaUtils.hpp>
#include <Standard.hpp>

inline const char* to_string(const mia::CppStandard e) {
	switch (e) {
		case mia::CppStandard::Cpp98: return "c++98";
		case mia::CppStandard::Cpp03: return "c++03";
		case mia::CppStandard::Cpp11: return "c++11";
		case mia::CppStandard::Cpp14: return "c++14";
		case mia::CppStandard::Cpp1z: return "c++1z";
		case mia::CppStandard::Cpp17: return "c++17";
		case mia::CppStandard::Cpp2a: return "c++2a";
		case mia::CppStandard::Cpp20: return "c++20";
	}
}

template<> inline mia::CppStandard to_enum(const std::string& str) {
	static const std::unordered_map<std::string, mia::CppStandard> mapping = {
		{ "c++98", mia::CppStandard::Cpp98 },
		{ "c++03", mia::CppStandard::Cpp03 },
		{ "c++11", mia::CppStandard::Cpp11 },
		{ "c++14", mia::CppStandard::Cpp14 },
		{ "c++1z", mia::CppStandard::Cpp1z },
		{ "c++17", mia::CppStandard::Cpp17 },
		{ "c++2a", mia::CppStandard::Cpp2a },
		{ "c++20", mia::CppStandard::Cpp20 }
	};
	const auto it = mapping.find(str);
	if (it == mapping.end())
		throw std::invalid_argument("Cannot convert given string to enum.");
	return it->second;
};

template<> inline std::vector<mia::CppStandard> enum_to_list(const mia::CppStandard& v) {
	return {
		mia::CppStandard::Cpp98,
		mia::CppStandard::Cpp03,
		mia::CppStandard::Cpp11,
		mia::CppStandard::Cpp14,
		mia::CppStandard::Cpp1z,
		mia::CppStandard::Cpp17,
		mia::CppStandard::Cpp2a,
		mia::CppStandard::Cpp20
	};
};
#endif