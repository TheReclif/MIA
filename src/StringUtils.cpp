#include <StringUtils.hpp>

namespace mia::text
{
	std::string implode(const std::vector<std::string>& array, const std::string& delim)
	{
		std::string ret;

		bool first = true;

		for (const auto& s : array)
		{
			if (!first)
				ret += delim;
			ret += s;
			first = false;
		}

		return ret;
	}
	std::string pad(const std::string& str, int count, char delim)
	{
		return std::string(count, delim) + str;
	}
	std::vector<std::string> process(const std::vector<std::string>& array, const std::function<std::string(const std::string&)>& processor)
	{
		std::vector<std::string> ret;

		ret.reserve(array.size());

		for (const auto& s : array)
		{
			ret.emplace_back(processor(s));
		}

		return ret;
 	}
}