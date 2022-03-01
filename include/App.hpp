#pragma once
#ifndef APP_INCLUDED
#define APP_INCLUDED

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <atomic>

#include <Standard_Gen.hpp>

namespace mia
{
	struct AppConfig
	{
		int threadCount = -1;
		CppStandard cppStandard = CppStandard::Cpp11;
		bool dry = false;
	};

	class App
	{
	public:
		App() = delete;
		App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const AppConfig& config);
		App(const App&) = delete;
		App(App&&) noexcept = delete;

		~App() = default;

		App& operator=(const App&) = delete;
		App& operator=(App&&) noexcept = delete;

		void process();

	private:
		std::optional<std::string_view> getNextFileToProcess();
		void threadFunc();

		const std::vector<std::string> filesToProcess, includeDirs;
		const std::string pattern;
		std::atomic<unsigned int> currentProcessedFile;
		AppConfig config;
	};
}

#endif
