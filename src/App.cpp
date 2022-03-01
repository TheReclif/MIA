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
	mia::App::App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const GeneratorConfig& config)
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

	void App::registerModule(const GeneratorModule::Ptr& module)
	{
		modules.emplace_back(module);
	}

	void App::registerModules(const std::vector<GeneratorModule::Ptr>& modules)
	{
		this->modules.reserve(this->modules.size() + modules.size());
		for (auto& module : modules)
			this->modules.emplace_back(module);
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
		static const auto header_start_pattern = R"(
#pragma once
#ifndef {0}_INCLUDED
#define {0}_INCLUDED

#include <MiaUtils.hpp>
#include <{1}>
)";
		static const auto header_end = "#endif";

		Generator generator { config, includeDirs };
		generator.registerModules(modules);

		const auto fileCount = filesToProcess.size();

		decltype(getNextFileToProcess()) currentFile;

		while (currentFile = getNextFileToProcess())
		{
			const auto filePath = static_cast<std::string>(currentFile.value());
			
			std::optional<std::ofstream> outFile;

			const auto path = std::filesystem::path(filePath);

			const auto fileStem = path.stem().string();
			const auto fileName = path.filename().string();

			if (!config.dry)
				outFile = std::ofstream(fmt::format(pattern, fileStem));

			std::ostream& outStream = outFile ? outFile.value() : std::cout;

			if (!config.dry)
				outStream << fmt::format(header_start_pattern, text::toUpper(fileStem), fileStem);

			generator.generate(outStream, filePath);


			if (!config.dry)
				outStream << header_end;
		}
	}
}