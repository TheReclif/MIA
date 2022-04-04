#include <modules/Serialization.hpp>

#include <cppast/visitor.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_class.hpp>

#include <CppastUtils.hpp>

#include <mutex>
#include <sstream>

struct MembrVar
{
	const cppast::cpp_member_variable* var;
	cppast::cpp_access_specifier_kind access;
};

static void writeAttributesInitList(std::ostream& out, const cppast::cpp_entity& e)
{
	out << "{";
	for (const auto& attr : e.attributes())
	{
		out << "mia::Attribute(\"" << attr.name() << "\", {";
		if (attr.arguments().has_value())
		{
			for (const auto& arg : attr.arguments().value())
			{
				out << "\"" << arg.spelling << "\", ";
			}
		}

		out << "}), ";
	}
	out << "}";
}

static void generateCodeForClass(std::ostream& out, const std::pair<const cppast::cpp_class*, std::vector<MembrVar>>& x)
{
	using namespace mia;

	const auto className = utils::getEntityFullyQualifiedName(*x.first);
	const auto isClassTemplate = utils::isTemplate(*x.first);

	if (!isClassTemplate)
	{
		out << "namespace detail { template<> struct NameOf<" << className << "> { static const char* name() { return \"" << x.first->name() << "\"; }};}\n";
		out << "namespace detail { template<> struct TypeOf<" << className << ">\n{\n";
	}
	else
	{
		out << "namespace detail { template<class... Args> struct NameOf<" << className << "<Args...>> { static const char* name() { return \"" << x.first->name() << "\"; }};}\n";
		out << "namespace detail { template<class... Args> struct TypeOf<" << className << "<Args...>>\n{\n";
	}
	out << "\tstatic const mia::Type& type()\n\t{\n";
	out << "\t\tstatic mia::Type type(mia::detail::Tag<" << className << (isClassTemplate ? "<Args...>>(), \"" : ">(), \"") << className << "\", mia::Type::Kind::Class, {\n";
	for (const auto& member : x.second)
	{
		out << "\t\t\tmia::Field(mia::AccessSpecifier::";
		switch (member.access)
		{
		case cppast::cpp_access_specifier_kind::cpp_private:
			out << "Private";
			break;
		case cppast::cpp_access_specifier_kind::cpp_protected:
			out << "Protected";
			break;
		case cppast::cpp_access_specifier_kind::cpp_public:
			out << "Public";
			break;
		}
		out << ", \"" << member.var->name() << "\", \"" << utils::getTypeName(member.var->type()) << "\", ";
		writeAttributesInitList(out, *member.var);
		out << "),\n";
	}
	out << "\t\t\t},\n\t\t\t";
	writeAttributesInitList(out, *x.first);
	out << ",\n\t\t\t{";
	for (const auto& x : x.first->bases())
	{
		// HACK
		if (!utils::isTemplate(x.parent().value()))
		{
			out << "typeOf<" << utils::getEntityFullyQualifiedName(x) << ">(), ";
		}
	}
	out << "});\n";
	out << "\t\treturn type;\n";
	out << "\t}\n};\n";
	out << "}\n";
	if (!isClassTemplate)
	{
		out << "namespace mia {\n";
		out << "\tnamespace detail {\n";
		out << "\t\ttemplate class AutoRegisterChild<::" << className << ">;\n";
		out << "\t}\n";
		out << "}\n";
	}
}

namespace mia::modules
{
	void SerializationModule::extractInfo(std::ostream& output, cppast::cpp_file& cppFile)
	{
		thread_local std::unordered_map<const cppast::cpp_class*, std::vector<MembrVar>> classes;
		thread_local cppast::cpp_access_specifier_kind currentAccess;

		cppast::visit(cppFile, cppast::whitelist<cppast::cpp_entity_kind::member_variable_t, cppast::cpp_entity_kind::class_t, cppast::cpp_entity_kind::access_specifier_t>(),
			[&](const cppast::cpp_entity& e, const cppast::visitor_info visitInfo) -> bool
			{
				switch (e.kind())
				{
				case cppast::cpp_entity_kind::class_t:
				{
					const cppast::cpp_class& cl = static_cast<const cppast::cpp_class&>(e);
					if (cl.is_definition())
					{
						classes.emplace(std::piecewise_construct, std::forward_as_tuple(&cl), std::tuple<>());
						currentAccess = cppast::cpp_access_specifier_kind::cpp_private;
					}
				}
					return true;
				case cppast::cpp_entity_kind::access_specifier_t:
				{
					const cppast::cpp_access_specifier& accSpec = static_cast<const cppast::cpp_access_specifier&>(e);
					currentAccess = accSpec.access_specifier();
				}
					return true;
				}

				const cppast::cpp_member_variable& memberVariable = static_cast<const cppast::cpp_member_variable&>(e);

				auto& parent = memberVariable.parent().value();
				switch (parent.kind())
				{
				case cppast::cpp_entity_kind::class_t:
					classes[static_cast<const cppast::cpp_class*>(&memberVariable.parent().value())].push_back({ &memberVariable, currentAccess });
					break;
				}

				return true;
			}
		);

		static std::mutex mut;

		std::lock_guard<std::mutex> _(mut);
		for (const auto& x : classes)
		{
			generateCodeForClass(output, x);
		}
	}
}
