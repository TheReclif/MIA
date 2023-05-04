#pragma once
#ifndef MIA_GENERATOR_INCLUDED
#define MIA_GENERATOR_INCLUDED

#include <iostream>
#include <cppast/libclang_parser.hpp>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

#include <Standard.hpp>

#ifdef WIN32
#define MiaExportModule(name) extern "C" inline __declspec(dllexport) mia::GeneratorModule::Ptr mia_exportModule() { return std::make_shared<name>(); }
#else
#define MiaExportModule(name) extern "C" inline mia::GeneratorModule::Ptr mia_exportModule() { return std::make_shared<name>(); }
#endif

namespace argumentum
{
	class ParameterConfig;
}

namespace mia
{
	struct DynamicLibrary;

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
		using CreateFunc = Ptr(*)();

		virtual ~GeneratorModule() = default;

		virtual void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) = 0;
		[[nodiscard]]
		virtual const char* getVersion() const = 0;

		static Ptr loadFromLibrary(const DynamicLibrary& lib);
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