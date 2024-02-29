#include <argumentum/argparse.h>

#include <spdlog/spdlog.h>

#include <vector>
#include <string>

#include <App.hpp>
#include <StringUtils.hpp>
#include <Standard_Gen.hpp>
#include <DynamicLibrary.hpp>

#include <modules/EnumConversions.hpp>
#include <modules/Serialization.hpp>

int main(int argc, char** argv)
try
{
	// Setup the tool's arguments.
	std::vector<std::string> filesToProcess, includeDirs, modules;
	std::string outputPattern, cxxStd;
	bool showConfig;

	argumentum::argument_parser parser;
	auto params = parser.params();

	mia::GeneratorConfig config;

	parser.config().program(argv[0]).description("Meta Information Appender");

	params
		.add_parameter(filesToProcess, "--files", "-f").minargs(1).metavar("<files>")
		.help("Files to process.").required(true);
	params
		.add_parameter(includeDirs, "--includes", "-i").minargs(1).metavar("<include_dirs>")
		.help("Include directories.").required(false);
	params
		.add_parameter(outputPattern, "--output", "-o").metavar("<output_pattern>")
		.help("Output pattern for the output files. Defaults to \"{}.mia.hpp\" where {} is a placeholder for the input file name.").required(false).absent("{}.mia.hpp").nargs(1);
	params
		.add_parameter(modules, "--modules", "-m").metavar("<modules>")
		.help("Modules to load alongside the default ones.").required(false).minargs(1);
	params
		.add_parameter(showConfig, "--show-config", "-?").metavar("<show_config>")
		.help("Show config specified to MIA. Usefull for debugging the configuration.").required(false).absent("false");

	config.registerOptions(params);

	auto res = parser.parse_args(argc, argv);
	if (!res)
	{
		for (const auto& x : res.errors)
		{
			x.describeError(std::cout);
		}
		return 1;
	}

	std::vector<std::unique_ptr<mia::DynamicLibrary>> dynamicLibs;
	dynamicLibs.reserve(modules.size());
 	for (const auto& x : modules)
	{
		dynamicLibs.push_back(mia::DynamicLibraryFactory::create());
		if (!dynamicLibs.back()->load(x.c_str()))
		{
			spdlog::error("Failed to load module {}", x);
			return 1;
		}
	}

	if (showConfig)
	{
		spdlog::info("Modules loaded: {}", mia::text::implode(modules, ", "));
		spdlog::info("Included directories: {}", mia::text::implode(includeDirs, ", "));
		spdlog::info("Input files: {}", mia::text::implode(filesToProcess, ", "));
		spdlog::info("Output pattern: {}", outputPattern);
		config.log();
	}

	mia::App app(std::move(filesToProcess), std::move(includeDirs), std::move(outputPattern), config);

	app.registerModules({
		std::make_shared<mia::modules::EnumConversionsModule>(),
		std::make_shared<mia::modules::SerializationModule>()
		});

	for (const auto& x : dynamicLibs)
	{
		const auto miaModule = mia::GeneratorModule::loadFromLibrary(*x);
		if (miaModule)
		{
			app.registerModule(miaModule);
		}
	}

	if (!app.process())
	{
		return 1;
	}

	if (config.verbose)
		spdlog::info("Done");

	return 0;
}
catch (...)
{
	std::cout << "Unexpected exception" << std::endl;
	return 1;
}
