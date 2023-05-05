#pragma once
#ifndef APP_INCLUDED
#define APP_INCLUDED

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <atomic>
#include <filesystem>

#include <Generator.hpp>

namespace mia
{
	class App
	{
	public:
		struct File
		{
			std::filesystem::path path;
			std::string content;
		};
	public:
		App() = delete;
		App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const mia::GeneratorConfig& config);
		App(const App&) = delete;
		App(App&&) noexcept = delete;

		~App() = default;

		App& operator=(const App&) = delete;
		App& operator=(App&&) noexcept = delete;

		bool process();
		void registerModule(const GeneratorModule::Ptr& module);
		void registerModule(GeneratorModule* module);
		void registerModules(const std::vector<GeneratorModule::Ptr>& modules);
	private:
		std::optional<std::string_view> getNextFileToProcess();
		void threadFunc(std::vector<File>& output, bool& error);
		void applyFiles(const std::vector<File>& files) const;

		const std::vector<std::string> filesToProcess, includeDirs;
		const std::string pattern;
		std::atomic<unsigned int> currentProcessedFile;
		GeneratorConfig config;
		std::vector<GeneratorModule::Ptr> ownedModules;
		std::vector<GeneratorModule*> modules;
	};
}

#endif
