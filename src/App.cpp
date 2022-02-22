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

void mia::App::threadFunc()
{
	const auto fileCount = filesToProcess.size();
	unsigned int currentFile = currentProcessedFile++;

	cppast::libclang_compile_config compileConfig;
	cppast::cpp_standard compileStandard;
	cppast::compile_flags flags;
#define CHECK(a, b) case a: compileStandard = b; break;
	switch (cppStandard)
	{
		CHECK(CppStandard::Cpp98, cppast::cpp_standard::cpp_98);
		CHECK(CppStandard::Cpp03, cppast::cpp_standard::cpp_03);
		CHECK(CppStandard::Cpp11, cppast::cpp_standard::cpp_11);
		CHECK(CppStandard::Cpp14, cppast::cpp_standard::cpp_14);
		CHECK(CppStandard::Cpp1z, cppast::cpp_standard::cpp_1z);
		CHECK(CppStandard::Cpp17, cppast::cpp_standard::cpp_17);
		CHECK(CppStandard::Cpp2a, cppast::cpp_standard::cpp_2a);
		CHECK(CppStandard::Cpp20, cppast::cpp_standard::cpp_20);
	default:
		return;
	}
#undef CHECK
	compileConfig.set_flags(compileStandard, flags);
	for (const auto& x : includeDirs)
	{
		compileConfig.add_include_dir(x);
	}

	cppast::libclang_parser parser;
	while (currentFile < fileCount)
	{
		cppast::cpp_entity_index idx;

		auto file = parser.parse(idx, filesToProcess[currentFile], compileConfig);
		if (parser.error())
		{
			spdlog::error("Unable to parse {}", filesToProcess[currentFile]);
			return;
		}

		spdlog::info("{} parsed", filesToProcess[currentFile]);
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

			std::string eName;
			for (const auto& x : namespaceStack)
			{
				eName += x;
				eName += "::";
			}
			eName += eEnum.name();
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

			return true;
		});

		currentFile = currentProcessedFile++;
		parser.reset_error();
	}
}

mia::App::App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const int threads, const CppStandard standard)
	: filesToProcess(std::move(files)), includeDirs(std::move(includes)), pattern(std::move(outputPattern)), threadCount(threads), cppStandard(standard)
{}

void mia::App::process()
{
	currentProcessedFile = 0u;

	const unsigned int threadsToUse = (threadCount > 0) ? threadCount : std::min(std::thread::hardware_concurrency(), static_cast<unsigned int>(filesToProcess.size()));
	std::vector<std::thread> threads;
	threads.reserve(threadsToUse);
	for (unsigned int x = 0; x < threadsToUse; ++x)
	{
		threads.emplace_back([this]() { threadFunc(); });
	}

	for (auto& x : threads)
	{
		if (x.joinable())
		{
			x.join();
		}
	}
}
