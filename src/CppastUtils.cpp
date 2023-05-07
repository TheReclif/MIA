#include <CppastUtils.hpp>

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_class_template.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_array_type.hpp>

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

	std::string getVariableWithTypeName(const cppast::cpp_member_variable& var)
	{
		std::string result(cppast::to_string(var.type()));

		const auto pos = result.find('[');
		if (pos != std::string::npos)
		{
			result.insert(pos, " " + var.name());
			return result;
		}

		return result + " " + var.name();

		/*std::string midSpec, postSpec;

		const auto& varType = var.type();
		switch (varType.kind())
		{
		case cppast::cpp_type_kind::array_t:
		{
			const auto& arr = static_cast<const cppast::cpp_array_type&>(varType);
			
		}
		break;
		case cppast::cpp_type_kind::pointer_t:
		{

		}
			break;
		}*/
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
