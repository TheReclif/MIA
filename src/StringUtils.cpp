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

	std::string toUpper(const std::string& str)
	{
		std::string ret{ str };
		std::transform(ret.begin(), ret.end(), ret.begin(), std::toupper);
		return ret;
	}
}