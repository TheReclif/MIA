#include <modules/EnumConversions.hpp>

#include <cppast/cpp_enum.hpp>
#include <cppast/visitor.hpp>

#include <fmt/format.h>

#include <StringUtils.hpp>
#include <CppastUtils.hpp>

static const auto to_string_main_pattern = R"TXT(
inline const char* to_string(const {} e) {{
	switch (e) {{
{}
	}}
}}
)TXT";
static const auto to_string_item_pattern = "\t\tcase {}: return {};";

static const auto to_enum_main_pattern = R"TXT(
template<> inline {0} to_enum(const std::string& str) {{
	static const std::unordered_map<std::string, {0}> mapping = {{
{1}
	}};
	const auto it = mapping.find(str);
	if (it == mapping.end())
		throw std::invalid_argument("Cannot convert given string to enum.");
	return it->second;
}};
)TXT";

static const auto to_enum_item_pattern = "\t\t{{ {}, {} }}";

namespace mia::modules
{
	void EnumConversionsModule::extractInfo(std::ostream& outputStream, cppast::cpp_file& source)
	{
		cppast::visit(source, cppast::whitelist<cppast::cpp_entity_kind::enum_t>(),
			[this, &outputStream](const cppast::cpp_entity& e, cppast::visitor_info info) -> bool
			{
				if (info.is_old_entity() || !(cppast::has_attribute(e, "mia::include")))
				{
					return true;
				}

				const auto& eEnum = static_cast<const cppast::cpp_enum&>(e);
				if (eEnum.is_declaration())
				{
					return true;
				}

				const auto nsName = utils::getEntityParentFullName(e);

				std::string eName = nsName + eEnum.name();

				using EnumOption = std::pair<std::string, std::string>;

				std::vector<EnumOption> enumOptions;
				for (const auto& x : eEnum)
				{
					std::string valName;
					if (auto attr = cppast::has_attribute(x, "mia::name"))
					{
						valName = attr.value().arguments().value().as_string();
					}
					else
					{
						valName = "\"" + x.name() + "\"";
					}
					enumOptions.emplace_back(eName + "::" + x.name(), valName);
				}
				outputStream << fmt::format(
					to_string_main_pattern,
					eName,
					text::implode(text::process<EnumOption>(
						enumOptions,
						[](const EnumOption& option) {
							return fmt::format(to_string_item_pattern, option.first, option.second);
						}
						), "\n")
				);

				outputStream << fmt::format(
					to_enum_main_pattern,
					eName,
					text::implode(text::process<EnumOption>(
						enumOptions,
						[](const EnumOption& option) {
							return fmt::format(to_enum_item_pattern, option.second, option.first);
						}
						), ",\n")
				);

				return true;
			});
	}

	const char* EnumConversionsModule::getVersion() const
	{
		return MIA_VERSION;
	}
}
