#include <App.hpp>
#include <CodeGenerator.hpp>

#include <argumentum/argparse.h>
#include <spdlog/spdlog.h>

#include <cppast/cpp_entity_kind.hpp>        // for the cpp_entity_kind definition
#include <cppast/cpp_forward_declarable.hpp> // for is_definition()
#include <cppast/cpp_namespace.hpp>          // for cpp_namespace
#include <cppast/libclang_parser.hpp> // for libclang_parser, libclang_compile_config, cpp_entity,...
#include <cppast/visitor.hpp>         // for visit()
#include <cppast/cpp_enum.hpp> // cpp_enum

#include <deque>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include <StringUtils.hpp>

namespace mia
{
	mia::App::App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const GeneratorConfig& config)
		: filesToProcess(std::move(files)), includeDirs(std::move(includes)), pattern(std::move(outputPattern)), config(config)
	{}

	bool mia::App::process()
	{
		currentProcessedFile = 0u;

		if (filesToProcess.size() <= 1 || config.threadCount == 1)
		{
			bool error;
			std::vector<File> out;
			threadFunc(out, error);
			if (error)
			{
				return false;
			}
			applyFiles(out);
			return true;
		}
		
		const unsigned int threadsToUse = std::min(
			static_cast<unsigned int>(filesToProcess.size()),
			(config.threadCount > 0) ? config.threadCount : std::thread::hardware_concurrency()
		);
		std::vector<char> errors(threadsToUse);
		std::vector<std::vector<File>> out(threadsToUse);
		std::vector<std::thread> threads;
		threads.reserve(threadsToUse);
		for (unsigned int x = 0; x < threadsToUse; ++x)
		{
			threads.emplace_back(&App::threadFunc, this, std::ref(out[x]), std::ref(reinterpret_cast<bool&>(errors[x])));
		}

		for (auto& x : threads)
		{
			if (x.joinable())
			{
				x.join();
			}
		}

		for (const auto x : errors)
		{
			if (x)
			{
				return false;
			}
		}

		for (const auto& x : out)
		{
			applyFiles(x);
		}

		return true;
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

	void mia::App::threadFunc(std::vector<File>& output, bool& error)
	try
	{
		error = false;
		static const auto headerStartPattern = R"(
#pragma once
#ifndef {0}_INCLUDED
#define {0}_INCLUDED

#include <MiaUtils.hpp>
#include <{1}>
)";
		static const auto headerEnd = "#endif";
		
		Generator generator { config, includeDirs };
		generator.registerModules(modules);

		while (auto currentFile = getNextFileToProcess())
		{
			const auto filePath = static_cast<std::string>(currentFile.value());
			
			const auto path = std::filesystem::path(filePath);

			const auto fileStem = path.stem().string();
			const auto fileName = path.filename().string();
			
			std::ostringstream outStream;

			if (!config.dry)
				outStream << fmt::format(headerStartPattern, text::toUpper(fileStem), fileName);

			generator.generate(outStream, filePath);

			if (!config.dry)
				outStream << headerEnd;

			output.emplace_back(File{ path, outStream.str() });
		}
	}
	catch (...)
	{
		error = true;
	}

	void App::applyFiles(const std::vector<File>& files) const
	{
		for (const auto& file : files)
		{
			std::ofstream outFile;

			const auto& path = file.path;
			const auto fileStem = path.stem().string();
			const auto fileName = path.filename().string();

			if (!config.dry)
				outFile.open(fmt::format(pattern, fileStem));

			std::ostream& outStream = outFile ? outFile : std::cout;

			if (!config.dry)
				outStream << file.content;
		}
	}
}