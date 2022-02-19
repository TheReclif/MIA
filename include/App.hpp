#pragma once
#ifndef APP_INCLUDED
#define APP_INCLUDED

#include <vector>
#include <string>
#include <atomic>

namespace mia
{
	class App
	{
	public:
		enum class CppStandard
		{
			Cpp98,
			Cpp03,
			Cpp11,
			Cpp14,
			Cpp1z,
			Cpp17,
			Cpp2a,
			Cpp20
		};
	private:
		const std::vector<std::string> filesToProcess, includeDirs;
		const std::string pattern;
		std::atomic<unsigned int> currentProcessedFile;
		int threadCount = -1;
		CppStandard cppStandard;
	private:
		void threadFunc();
	public:
		App() = delete;
		App(std::vector<std::string>&& files, std::vector<std::string>&& includes, std::string&& outputPattern, const int threads, const CppStandard standard);
		App(const App&) = delete;
		App(App&&) noexcept = delete;

		~App() = default;

		App& operator=(const App&) = delete;
		App& operator=(App&&) noexcept = delete;

		void process();
	};
}

#endif
