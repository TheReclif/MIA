#include <App.hpp>

#include <thread>
#include <fstream>

std::string readWholeFile(const std::string& filename)
{
	std::ifstream is;
	is.open(filename);
	if (!is.is_open())
	{
		return "";
	}

	// TODO
}

void mia::App::threadFunc()
{
	const auto fileCount = filesToProcess.size();
	unsigned int currentFile = currentProcessedFile++;
	while (currentFile < fileCount)
	{
		const std::string currentFileName = filesToProcess[currentFile];
		// TODO

		currentFile = currentProcessedFile++;
	}
}

mia::App::App(std::vector<std::string>&& files, std::string&& outputPattern, const int threads)
	: filesToProcess(std::move(files)), pattern(std::move(outputPattern)), threadCount(threads)
{}

void mia::App::process()
{
	currentProcessedFile = 0u;

	const unsigned int threadsToUse = (threadCount > 0) ? threadCount : std::thread::hardware_concurrency();
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
