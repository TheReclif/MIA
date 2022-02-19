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
	private:
		const std::vector<std::string> filesToProcess;
		const std::string pattern;
		std::atomic<unsigned int> currentProcessedFile;
		int threadCount = -1;
	private:
		void threadFunc();
	public:
		App() = delete;
		App(std::vector<std::string>&& files, std::string&& outputPattern, const int threads);
		App(const App&) = delete;
		App(App&&) noexcept = delete;

		~App() = default;

		App& operator=(const App&) = delete;
		App& operator=(App&&) noexcept = delete;

		void process();
	};
}

#endif
