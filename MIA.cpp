#include <argumentum/argparse.h>

#include <vector>
#include <string>

#include <App.hpp>
#include <Standard_Gen.hpp>

#include <modules/EnumConversions.hpp>

int main(int argc, char** argv)
{
	// Setup the tool's arguments.
	std::vector<std::string> filesToProcess, includeDirs;
	std::string outputPattern;

	argumentum::argument_parser parser;
	auto params = parser.params();

	mia::GeneratorConfig config;

	parser.config().program(argv[0]).description("Meta Information Appender");

	params
		.add_parameter(filesToProcess, "--files", "-f").minargs(1).metavar("<files>")
		.help("Files to process").required(true);
	params
		.add_parameter(includeDirs, "--includes", "-i").minargs(1).metavar("<include_dirs>")
		.help("Include directories").required(false);
	params
		.add_parameter(outputPattern, "--output", "-o").metavar("<output_pattern>")
		.help("Output pattern for the output files. Defaults to \"{}.mia.hpp\" where {} is a placeholder for the input file name").required(false).absent("{}.mia.hpp").nargs(1);
	
	config.registerOptions(params);

	auto res = parser.parse_args(argc, argv);
	if (!res)
		return 1;

	mia::App app(std::move(filesToProcess), std::move(includeDirs), std::move(outputPattern), config);

	app.registerModules({
		std::make_shared<mia::modules::EnumConversionsModule>()
	});

	app.process();

	return 0;
}
