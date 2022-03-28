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

static void generateCodeForClass(std::ostream& out, const std::pair<const cppast::cpp_entity*, std::vector<MembrVar>>& x)
{
	using namespace mia;

	const auto className = utils::getEntityFullyQualifiedName(*x.first);

	out << "template<> constexpr const char* nameOf<" << className << ">() { return \"" << x.first->name() << "\"; }\n";
	out << "template<> const mia::Type& typeOf<" << className << ">()\n{\n";
	out << "\tstatic mia::Type type(mia::detail::Tag<" << className << ">(), \"" << className << "\", mia::Type::Kind::Class, {\n";
	for (const auto& member : x.second)
	{
		out << "\t\tmia::Field(mia::AccessSpecifier::";
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
	out << "\t\t},\n\t\t";
	writeAttributesInitList(out, *x.first);
	out << ");\n";
	out << "\treturn type;\n";
	out << "}\n";
	out << "namespace mia {\n";
	out << "\tnamespace detail {\n";
	out << "\t\ttemplate class AutoRegisterChild<::" << className << ">;\n";
	out << "\t}\n";
	out << "}\n";
}

namespace mia::modules
{
	void SerializationModule::extractInfo(std::ostream& output, cppast::cpp_file& cppFile)
	{
		thread_local std::unordered_map<const cppast::cpp_entity*, std::vector<MembrVar>> classes;
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

				classes[&memberVariable.parent().value()].push_back({ &memberVariable, currentAccess });

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
