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

	while (currentFile < fileCount)
	{
		const std::string fileContents = readWholeFile(filesToProcess[currentFile]);
		// TODO

		

		currentFile = currentProcessedFile++;
	}
}

mia::App::App(std::vector<std::string>&& files, std::string&& outputPattern, const int threads, const CppStandard standard)
	: filesToProcess(std::move(files)), pattern(std::move(outputPattern)), threadCount(threads), cppStandard(standard)
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
