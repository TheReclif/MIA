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
#include <unordered_set>
#include <optional>
#include <filesystem>

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

	void AppConfig::registerOptions(argumentum::ParameterConfig& params)
	{
		params
			.add_parameter(threadCount, "--threads", "-t")
			.metavar("<threads>")
			.help("How many threads to start")
			.required(false).absent(-1).nargs(1);
		params
			.add_parameter(cppStandard, "--std")
			.metavar("<std>")
			.help("C++ standard to compile against")
			.required(false).absent(CppStandard::Cpp11).nargs(1)
			.choices({ "c++98", "c++03", "c++11", "c++14", "c++1z", "c++17", "c++2a", "c++20" })
			.action([](auto& target, const std::string& value) 
				{
					target = to_enum<CppStandard>(value);
				});
		params
			.add_parameter(dry, "--dry-run", "-d")
			.help("Program in dry run will not modify any file and only print information about what it would do.")
			.required(false).absent(false);
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

		static const auto header_start_pattern = R"(
#pragma once
#ifndef {0}_INCLUDED
#define {0}_INCLUDED

#include <MiaUtils.hpp>
#include <{1}>
)";
		static const auto header_end = "#endif";

		while (currentFile = getNextFileToProcess())
		{
			cppast::cpp_entity_index idx;

			const auto filePath = static_cast<std::string>(currentFile.value());

			try
			{
				auto file = parser.parse(idx, filePath, compileConfig);
				if (parser.error())
				{
					spdlog::error("Unable to parse {}", filePath);
					return;
				}

				spdlog::info("{} parsed", filePath);
				
				std::optional<std::ofstream> outFile;

				const auto path = std::filesystem::path(filePath);

				const auto fileStem = path.stem().string();
				const auto fileName = path.filename().string();

				if (!config.dry)
					outFile = std::ofstream(fmt::format(pattern, fileStem));

				std::ostream& outStream = outFile ? outFile.value() : std::cout;

				if (outFile)
					outStream << fmt::format(header_start_pattern, text::toUpper(fileStem), fileName) << "\n";

				cppast::visit(*file, cppast::whitelist<cppast::cpp_entity_kind::enum_t, cppast::cpp_entity_kind::namespace_t>(),
					[this, &outStream](const cppast::cpp_entity& e, cppast::visitor_info info) -> bool
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

						if (info.is_old_entity() || !(cppast::has_attribute(e, "mia::include")))
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
							enumOptions.emplace_back(eName+"::"+x.name(), valName);
						}
						outStream << fmt::format(
							to_string_main_pattern,
							eName,
							text::implode(text::process<EnumOption>(
								enumOptions,
								[](const EnumOption& option) {
									return fmt::format(to_string_item_pattern, option.first, option.second);
								}
							), "\n")
						);

						outStream << fmt::format(
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
				if (outFile)
					outStream << header_end;
			}
			catch (std::exception e)
			{
				spdlog::error("{}", e.what());
			}
			parser.reset_error();
		}
	}
}