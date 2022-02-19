#include <App.hpp>

#include <spdlog/spdlog.h>

#include <cppast/code_generator.hpp>         // for generate_code()
#include <cppast/cpp_entity_kind.hpp>        // for the cpp_entity_kind definition
#include <cppast/cpp_forward_declarable.hpp> // for is_definition()
#include <cppast/cpp_namespace.hpp>          // for cpp_namespace
#include <cppast/libclang_parser.hpp> // for libclang_parser, libclang_compile_config, cpp_entity,...
#include <cppast/visitor.hpp>         // for visit()

#include <thread>
#include <fstream>

static std::string readWholeFile(const std::string& filename)
{
	std::ifstream is;
	is.open(filename);
	if (!is.is_open())
	{
		return "";
	}

	is.seekg(0, std::ios::end);
	const auto size = is.tellg();
	is.seekg(0, std::ios::beg);

	std::string result(size, '\0');
	is.read(result.data(), size);
	
	return result;
}

void mia::App::threadFunc()
{
	const auto fileCount = filesToProcess.size();
	unsigned int currentFile = currentProcessedFile++;

	while (currentFile < fileCount)
	{
		const std::string fileContents = readWholeFile(filesToProcess[currentFile]);
		// TODO

		spdlog::info("{}: {} bytes", filesToProcess[currentFile], fileContents.size());

		currentFile = currentProcessedFile++;
	}
}

mia::App::App(std::vector<std::string>&& files, std::string&& outputPattern, const int threads)
	: filesToProcess(std::move(files)), pattern(std::move(outputPattern)), threadCount(threads)
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
