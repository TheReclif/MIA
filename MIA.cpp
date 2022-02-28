#include <argumentum/argparse.h>

#include <vector>
#include <string>

#include <App.hpp>
#include <Standard_Gen.hpp>

int main(int argc, char** argv)
{
	using Standard = mia::CppStandard;

	// Setup the tool's arguments.
	std::vector<std::string> filesToProcess, includeDirs;
	std::string outputPattern, standard;

	argumentum::argument_parser parser;
	auto params = parser.params();

	mia::AppConfig config;

	parser.config().program(argv[0]).description("Meta Information Appender");
	
	params.add_parameter(config.threadCount, "--threads", "-t").metavar("<threads>").help("How many threads to start").required(false).absent(-1).nargs(1);
	params.add_parameter(standard, "--std").metavar("<std>").help("C++ standard to compile against").required(false).absent("c++17").nargs(1).choices({ "c++98", "c++03", "c++11", "c++14", "c++1z", "c++17", "c++2a", "c++20" });
	params.add_parameter(config.dry, "--dry-run", "-d").help("Program in dry run will not modify any file and only print information about what it would do.").required(false).absent(false);

	params.add_parameter(filesToProcess, "--files", "-f").minargs(1).metavar("<files>").help("Files to process").required(true);
	params.add_parameter(includeDirs, "--includes", "-i").minargs(1).metavar("<include_dirs>").help("Include directories").required(false);
	params.add_parameter(outputPattern, "--output", "-o").metavar("<output_pattern>").help("Output pattern for the output files. Defaults to \"{}.hpp\" where {} is a placeholder for the input file name").required(false).absent("{}.hpp").nargs(1);
	
	auto res = parser.parse_args(argc, argv);
	if (!res)
		return 1;
	
	config.cppStandard = to_enum<mia::CppStandard>(standard);

	mia::App app(std::move(filesToProcess), std::move(includeDirs), std::move(outputPattern), config);
	app.process();

	return 0;
}
