#pragma once
#ifndef MIA_STRING_UTILS_INCLUDED
#define MIA_STRING_UTILS_INCLUDED

#include <string>
#include <vector>
#include <functional>
#include <algorithm>

namespace mia::text
{
	std::string implode(const std::vector<std::string>& array, const std::string& delim);

	std::string pad(const std::string& str, int count, char delim);

	std::vector<std::string> process(const std::vector<std::string>& array, const std::function<std::string(const std::string&)>& processor);
}

#endif