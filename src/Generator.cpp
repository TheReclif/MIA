#include <Generator.hpp>
#include <optional>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <argumentum/argparse.h>

#include <cppast/visitor.hpp>
#include <cppast/cpp_enum.hpp>

#include <StringUtils.hpp>

#include <DynamicLibrary.hpp>
#include <Standard_Gen.hpp>

namespace mia
{
	void GeneratorConfig::registerOptions(argumentum::ParameterConfig& params)
	{
		params
			.add_parameter(threadCount, "--threads", "-t")
			.metavar("<threads>")
			.help("How many threads to start")
			.required(false).absent(0).nargs(1);
		params
			.add_parameter(cppStandard, "--std")
			.metavar("<std>")
			.help("C++ standard to compile against")
			.required(false).absent(CppStandard::Cpp11).nargs(1)
			.choices({ "c++98", "c++03", "c++11", "c++14", "c++1z", "c++17", "c++2a", "c++20" }) //TODO: get from enum
			.action([](auto& target, const std::string& value)
				{
					target = to_enum<CppStandard>(value);
				});
		params
			.add_parameter(dry, "--dry-run", "-d")
			.help("Program in dry run will not modify any file and only print information about what it would do.")
			.required(false).absent(false);
		params
			.add_parameter(verbose, "--verbose", "-v")
			.help("Adds verbose logging.")
			.required(false).absent(false);
		params
			.add_parameter(textOutput, "--text", "-$")
			.help("Output content of files to stdout instead of files. Segments begin with --<path>-- and end with --end--.")
			.required(false).absent(false);
	}
	void GeneratorConfig::log() const
	{
		spdlog::info("Verbose: {}", verbose);
		spdlog::info("Dry: {}", dry);
		spdlog::info("Text: {}", textOutput);
		spdlog::info("Cpp: {}", to_string(cppStandard));
		spdlog::info("Threads: {}", threadCount);
	}

	Generator::Generator(const GeneratorConfig& config, const std::vector<std::string>& includeDirs) : dry(config.dry), verbose(config.verbose)
	{
		const auto compileStandard = convertStandards(config.cppStandard);
		if (!compileStandard)
			throw std::logic_error("Incorrect state, invalid cpp version should be thrown earlier!");

		cppast::compile_flags flags;

		compileConfig.set_flags(compileStandard.value(), flags);
		for (const auto& x : includeDirs)
		{
			compileConfig.add_include_dir(x);
		}
	}
	
	void Generator::registerModule(GeneratorModule* module)
	{
		if (std::string_view(module->getVersion()) == MIA_VERSION)
		{
			modules.emplace_back(module);
		}
		else
		{
			spdlog::error("Mismatching versions for module {}, module version is {}, but mia version is {}", "PLACEHOLDER_UNIMPLEMENTED", module->getVersion(), MIA_VERSION);
		}
	}

	void Generator::registerModules(const std::vector<GeneratorModule*>& modules)
	{
		this->modules.reserve(this->modules.size() + modules.size());
		for (auto& module : modules)
			registerModule(module);
	}

	void Generator::generate(std::ostream& outputStream, const std::string& targetFile)
	{
		cppast::cpp_entity_index id;
		try
		{
			auto cppFile = parser.parse(id, targetFile, compileConfig);
			if (parser.error())
			{
				spdlog::error("Unable to parse {}", targetFile);
				throw std::exception("Parse error");
			}

			if (verbose)
				spdlog::info("{} parsed", targetFile);

			for (auto module : modules)
			{
				module->extractInfo(outputStream, *cppFile);
			}

			if (verbose)
				spdlog::info("Info from {} extracted", targetFile);
		}
		catch (const std::exception& e)
		{
			spdlog::error("{}", e.what());
			throw;
		}
		parser.reset_error();
	}

	std::optional<cppast::cpp_standard> Generator::convertStandards(CppStandard standard)
	{
		static const std::unordered_map<CppStandard, cppast::cpp_standard> standardMapping = {
			{ CppStandard::Cpp98, cppast::cpp_standard::cpp_98 },
			{ CppStandard::Cpp03, cppast::cpp_standard::cpp_03 },
			{ CppStandard::Cpp11, cppast::cpp_standard::cpp_11 },
			{ CppStandard::Cpp14, cppast::cpp_standard::cpp_14 },
			{ CppStandard::Cpp1z, cppast::cpp_standard::cpp_1z },
			{ CppStandard::Cpp17, cppast::cpp_standard::cpp_17 },
			{ CppStandard::Cpp2a, cppast::cpp_standard::cpp_2a },
			{ CppStandard::Cpp20, cppast::cpp_standard::cpp_20 },
		};

		auto it = standardMapping.find(standard);
		if (it == standardMapping.end())
			return {};

		return it->second;
	}

	GeneratorModule* GeneratorModule::loadFromLibrary(const DynamicLibrary& lib)
	{
		const auto funcAddr = reinterpret_cast<CreateFunc>(lib.getFuncAddress("mia_exportModule"));
		if (funcAddr)
		{
			return funcAddr();
		}
		spdlog::error("Unable to find export function in module {}", lib.getName());
		return nullptr;
	}
}