#include <CppastUtils.hpp>

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>

#include <deque>

namespace mia::utils
{
	std::string getEntityParentFullName(const cppast::cpp_entity& e)
	{
		std::deque<std::string> scopeStack;

		auto x = &e;
		while (auto p = x->parent())
		{
			x = &p.value();
			if (x->kind() == cppast::cpp_entity_kind::file_t)
			{
				break;
			}
			scopeStack.push_front(x->name());
		}

		std::string result = "::";

		for (const auto& x : scopeStack)
		{
			result += x;
			result += "::";
		}

		return result;
	}
}
