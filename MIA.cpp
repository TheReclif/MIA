#include <argumentum/argparse.h>

#include <vector>
#include <string>

#include <App.hpp>

int main(int argc, char** argv)
{
	// Setup the tool's arguments.
	std::vector<std::string> filesToProcess;
	std::string outputPattern, standard;
	int threadCount;

	argumentum::argument_parser parser;
	auto params = parser.params();

	parser.config().program(argv[0]).description("Meta Information Appender");
	params.add_parameter(filesToProcess, "files").minargs(1).metavar("FILES").help("Files to process");
	params.add_parameter(outputPattern, "--output", "-o").metavar("OUTPUT_PATTERN").help("Output pattern for the output files. Defaults to \"{}.hpp\" where {} is a placeholder for the input file name").required(false).absent("{}.hpp").nargs(1);
	params.add_parameter(threadCount, "--threads", "-t").metavar("THREADS").help("How many threads to start").required(false).absent(-1).nargs(1);
	params.add_parameter(standard, "--std").metavar("STD").help("C++ standard to compile against").required(false).absent("c++17").nargs(1);

	auto res = parser.parse_args(argc, argv);
	if (!res)
		return 1;
	mia::App::CppStandard cppStandard;
	
#ifdef CHECK
#error CHECK already defined
#endif
#define CHECK(str, flag) if (standard == str) cppStandard = flag;
	CHECK("c++98", mia::App::CppStandard::Cpp98)
	else CHECK("c++03", mia::App::CppStandard::Cpp03)
	else CHECK("c++11", mia::App::CppStandard::Cpp11)
	else CHECK("c++14", mia::App::CppStandard::Cpp14)
	else CHECK("c++1z", mia::App::CppStandard::Cpp1z)
	else CHECK("c++17", mia::App::CppStandard::Cpp17)
	else CHECK("c++2a", mia::App::CppStandard::Cpp2a)
	else CHECK("c++20", mia::App::CppStandard::Cpp20)
	else return 1;
#undef CHECK
	mia::App app(std::move(filesToProcess), std::move(outputPattern), threadCount, cppStandard);
	app.process();

	return 0;
}
