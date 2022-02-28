#include <App.hpp>
#include <CodeGenerator.hpp>

#include <spdlog/spdlog.h>

#include <cppast/cpp_entity_kind.hpp>        // for the cpp_entity_kind definition
#include <cppast/cpp_forward_declarable.hpp> // for is_definition()
#include <cppast/cpp_namespace.hpp>          // for cpp_namespace
#include <cppast/libclang_parser.hpp> // for libclang_parser, libclang_compile_config, cpp_entity,...
#include <cppast/visitor.hpp>         // for visit()
#include <cppast/cpp_enum.hpp> // cpp_enum

#include <thread>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <optional>

#include <StringUtils.hpp>

namespace mia
{
	std::optional<cppast::cpp_standard> convertStandard(CppStandard standard)
	{
		static const std::unordered_map<CppStandard, cppast::cpp_standard> standardMapping = {
			{ CppStandard::Cpp98, cppast::cpp_standard::cpp_98 },
			{ CppStandard::Cpp03, cppast::cpp_standard::cpp_03 },
			{ CppStandard::Cpp11, cppast::cpp_standard::cpp_11 },
			{ CppStandard::Cpp14, cppast::cpp_standard::cpp_14 },
			{ CppStandard::Cpp1z, cppast::cpp_standard::cpp_1z },
			{ CppStandard::Cpp17, cppast::cpp_standard::cpp_17 },
			{ CppStandard::Cpp2a, cppast::cpp_standard::cpp_2a },
			{ CppStandard::Cpp20, cppast::cpp_standard::cpp_20 },
		};

		auto it = standardMapping.find(standard);
		if (it == standardMapping.end())
			return {};

		return it->second;
	}

	void initCompileConfig(cppast::libclang_compile_config& compileConfig, const AppConfig& appConfig, const std::vector<std::string>& includeDirs)
	{
		const auto compileStandard = convertStandard(appConfig.cppStandard);
		if (!compileStandard)
			throw std::logic_error("Incorrect state, invalid cpp version should be thrown earlier!");
		
		cppast::compile_flags flags;

		compileConfig.set_flags(compileStandard.value(), flags);
		for (const auto& x : includeDirs)
		{
			compileConfig.add_include_dir(x);
		}
	}

	mia::App::App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const AppConfig& config)
		: filesToProcess(std::move(files)), includeDirs(std::move(includes)), pattern(std::move(outputPattern)), config(config)
	{}

	void mia::App::process()
	{
		currentProcessedFile = 0u;

		const unsigned int threadsToUse = std::min(
			static_cast<unsigned int>(filesToProcess.size()),
			(config.threadCount > 0) ? config.threadCount : std::thread::hardware_concurrency()
		);
		std::vector<std::thread> threads;
		threads.reserve(threadsToUse);
		for (unsigned int x = 0; x < threadsToUse; ++x)
		{
			threads.emplace_back(&App::threadFunc, this);
		}

		for (auto& x : threads)
		{
			if (x.joinable())
			{
				x.join();
			}
		}
	}

	std::optional<std::string_view> App::getNextFileToProcess()
	{
		unsigned int currentFile = currentProcessedFile++;
		if (currentFile >= filesToProcess.size())
			return {};

		return filesToProcess[currentFile];
	}

	void mia::App::threadFunc()
	{
		const auto fileCount = filesToProcess.size();

		cppast::libclang_compile_config compileConfig;

		initCompileConfig(compileConfig, config, includeDirs);

		cppast::libclang_parser parser;

		decltype(getNextFileToProcess()) currentFile;

		while (currentFile = getNextFileToProcess())
		{
			cppast::cpp_entity_index idx;

			const auto fileName = static_cast<std::string>(currentFile.value());

			try
			{
				auto file = parser.parse(idx, fileName, compileConfig);
				if (parser.error())
				{
					spdlog::error("Unable to parse {}", fileName);
					return;
				}

				spdlog::info("{} parsed", fileName);
				// TODO: visit all of the properties and generate meta information
				cppast::visit(*file, cppast::whitelist<cppast::cpp_entity_kind::enum_t, cppast::cpp_entity_kind::namespace_t>(),
					[this](const cppast::cpp_entity& e, cppast::visitor_info info) -> bool
					{
						thread_local std::vector<std::string> namespaceStack;
						if (e.kind() == cppast::cpp_entity_kind::namespace_t)
						{
							if (info.event == cppast::visitor_info::container_entity_enter)
							{
								namespaceStack.push_back(e.name());
							}
							else
							{
								namespaceStack.pop_back();
							}

							return true;
						}

						if (info.is_old_entity() || !(cppast::has_attribute(e, "mia_gen::enum_string")))
						{
							return true;
						}

						const auto& eEnum = static_cast<const cppast::cpp_enum&>(e);
						if (eEnum.is_declaration())
						{
							return true;
						}

						const auto nsName = text::implode(namespaceStack, "::");

						std::string eName = eEnum.name();
						if (!nsName.empty())
							eName = nsName + "::" + eName;

						std::string result = fmt::format(
							"inline const char* to_string(const {} e) {}",
							eName,
							"{\n"
							"\tswitch(e) {\n"
						);
						for (const auto& x : eEnum)
						{
							std::string valName;
							if (auto attr = cppast::has_attribute(x, "mia_gen::enum_val_name"))
							{
								valName = attr.value().arguments().value().as_string();
							}
							else
							{
								valName = "\"" + x.name() + "\"";
							}

							result += fmt::format("\tcase {}::{}: return {};\n", eName, x.name(), valName);
						}
						result += "\t}\n}\n";
						std::cout << result;

						std::string innerBody = fmt::format(
							"\tstatic const std::unordered_map<std::string, {}> mapping = {}",
							eName,
							"{\n"
						);
						bool first = true;
						for (const auto& x : eEnum)
						{
							std::string valName;
							if (auto attr = cppast::has_attribute(x, "mia_gen::enum_val_name"))
							{
								valName = attr.value().arguments().value().as_string();
							}
							else
							{
								valName = "\"" + x.name() + "\"";
							}
							if (!first)
								innerBody += ",\n";
							innerBody += "\t\t{ " + fmt::format("{}, {}::{}", valName, eName, x.name()) + " }";
							first = false;
						}
						innerBody += "\n\t};\n";
						innerBody += text::implode(text::process({
								"const auto it = mapping.find(str);",
								"if (it == mapping.end())",
								"\tthrow std::invalid_argument(\"Cannot convert given string to enum.\");",
								"return it->second;"
							}, [](const auto& str) { return text::pad(str, 1, '\t'); }), "\n"
						);

						result = fmt::format(
							"template<> {} to_enum(const std::string& str) {}",
							eName,
							"{\n" + innerBody + "\n};"
						);

						std::cout << result;

						return true;
					});
			}
			catch (std::exception e)
			{
				spdlog::error("{}", e.what());
			}
			parser.reset_error();
		}
	}
}