#include <argumentum/argparse.h>

#include <vector>
#include <string>

#include <App.hpp>

int main(int argc, char** argv)
{
	// Setup the tool's arguments.
	std::vector<std::string> filesToProcess;
	std::string outputPattern;
	int threadCount;

	argumentum::argument_parser parser;
	auto params = parser.params();

	parser.config().program(argv[0]).description("Meta Information Appender");
	params.add_parameter(filesToProcess, "files").minargs(1).metavar("FILES").help("Files to process");
	params.add_parameter(outputPattern, "--output", "-o").metavar("OUTPUT_PATTERN").help("Output pattern for the output files. Defaults to \"{}.hpp\" where {} is a placeholder for the input file name").required(false).absent("{}.hpp").nargs(1);
	params.add_parameter(threadCount, "--threads", "-t").metavar("THREADS").help("How many threads to start").required(false).absent(-1).nargs(1);

	auto res = parser.parse_args(argc, argv);
	if (!res)
		return 1;

	mia::App app(std::move(filesToProcess), std::move(outputPattern), threadCount);
	app.process();

	return 0;
}
