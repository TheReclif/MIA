#include <CppastUtils.hpp>

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_class_template.hpp>

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
			if (x->kind() == cppast::cpp_entity_kind::class_template_t)
			{
				continue;
			}
			scopeStack.push_front(x->name());
		}

		std::string result;

		for (const auto& x : scopeStack)
		{
			result += x;
			result += "::";
		}

		return result;
	}

	std::string getEntityFullyQualifiedName(const cppast::cpp_entity& e)
	{
		if (e.parent().has_value())
		{
			return getEntityParentFullName(e) + e.name();
		}

		return e.name();
	}

	std::string getTypeName(const cppast::cpp_type& type)
	{
		return cppast::to_string(type);
	}

	const cppast::cpp_class_template* isTemplate(const cppast::cpp_entity& e)
	{
		if (e.parent().has_value())
		{
			const auto& v = e.parent().value();
			switch (v.kind())
			{
				case cppast::cpp_entity_kind::class_template_t:
					return &static_cast<const cppast::cpp_class_template&>(v);
			}
		}

		return nullptr;
	}
}
