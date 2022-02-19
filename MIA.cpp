#include <argumentum/argparse.h>

#include <spdlog/spdlog.h>

#include <cppast/code_generator.hpp>         // for generate_code()
#include <cppast/cpp_entity_kind.hpp>        // for the cpp_entity_kind definition
#include <cppast/cpp_forward_declarable.hpp> // for is_definition()
#include <cppast/cpp_namespace.hpp>          // for cpp_namespace
#include <cppast/libclang_parser.hpp> // for libclang_parser, libclang_compile_config, cpp_entity,...
#include <cppast/visitor.hpp>         // for visit()

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
	params.add_parameter(outputPattern, "--output", "-o").metavar("OUTPUT_PATTERN").help("Output pattern for the output files. Defaults to \"{}.hpp\" where {} is a placeholder for the input file name").required(false).absent("{}.hpp");
	params.add_parameter(threadCount, "--threads", "-t").metavar("THREADS").help("How many threads to start").required(false).absent(-1);

	auto res = parser.parse_args(argc, argv);
	if (!res)
		return 1;

	mia::App app(std::move(filesToProcess), std::move(outputPattern), threadCount);
	app.process();

	return 0;
}
