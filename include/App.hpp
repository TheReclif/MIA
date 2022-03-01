#pragma once
#ifndef APP_INCLUDED
#define APP_INCLUDED

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <atomic>
#include <argumentum/argparse.h>

#include <Generator.hpp>

namespace mia
{
	class App
	{
	public:
		App() = delete;
		App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const mia::GeneratorConfig& config);
		App(const App&) = delete;
		App(App&&) noexcept = delete;

		~App() = default;

		App& operator=(const App&) = delete;
		App& operator=(App&&) noexcept = delete;

		void process();
		void registerModule(const GeneratorModule::Ptr& module);
		void registerModules(const std::vector<GeneratorModule::Ptr>& modules);
	private:
		std::optional<std::string_view> getNextFileToProcess();
		void threadFunc();

		const std::vector<std::string> filesToProcess, includeDirs;
		const std::string pattern;
		std::atomic<unsigned int> currentProcessedFile;
		GeneratorConfig config;
		std::vector<std::shared_ptr<GeneratorModule>> modules;
	};
}

#endif
