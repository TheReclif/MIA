#pragma once
#ifndef MIA_ATTRIBUTABLE_INCLUDED
#define MIA_ATTRIBUTABLE_INCLUDED

#include <map>
#include <string>
#include <vector>
#include <string_view>
#include <initializer_list>

namespace mia
{
	struct Attribute
	{
		std::string_view name;
		std::vector<std::string_view> arguments;

		inline Attribute(std::string_view name, std::initializer_list<std::string_view> l)
			: name(name), arguments(l)
		{}
	};

	class Attributable
	{
	public:
		inline Attributable(std::initializer_list<Attribute> l)
			: attributes()
		{
			for (auto&& x : l)
			{
				const auto temp = x.name;
				attributes.emplace(temp, std::move(x));
			}
		}

		inline const std::multimap<std::string_view, Attribute>& getAttributes() const
		{
			return attributes;
		}
	protected:
		std::multimap<std::string_view, Attribute> attributes;
	};
}

#endif
