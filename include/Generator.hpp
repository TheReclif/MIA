#pragma once
#ifndef MIA_GENERATOR_INCLUDED
#define MIA_GENERATOR_INCLUDED

#include <iostream>
#include <cppast/libclang_parser.hpp>
#include <vector>
#include <memory>
#include <string_view>
#include <argumentum/argparse.h>

#include <Standard.hpp>

namespace mia
{
	class GeneratorConfig
	{
	public:
		int threadCount = -1;
		CppStandard cppStandard = CppStandard::Cpp11;
		bool dry = false;

		void registerOptions(argumentum::ParameterConfig& params);
	};

	class GeneratorModule
	{
	public:
		using Ptr = std::shared_ptr<GeneratorModule>;

		virtual ~GeneratorModule() = default;

		virtual void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) = 0;
		[[nodiscard]]
		virtual const char* getVersion() const = 0;
	};

	class Generator
	{
	public:
		Generator(const mia::GeneratorConfig& config, const std::vector<std::string>& includeDirs);

		void registerModule(const GeneratorModule::Ptr& module);
		void registerModules(const std::vector<GeneratorModule::Ptr>& modules);

		void generate(std::ostream& outputStream, const std::string& targetFile);
	private:
		static std::optional<cppast::cpp_standard> convertStandards(CppStandard standard);

		cppast::libclang_compile_config compileConfig;
		std::vector<GeneratorModule::Ptr> modules;
		cppast::libclang_parser parser;

		bool dry;
	};
}

#endif