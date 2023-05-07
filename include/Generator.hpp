#pragma once
#ifndef MIA_GENERATOR_INCLUDED
#define MIA_GENERATOR_INCLUDED

#include <core_export.h>

#include <iostream>
#include <cppast/libclang_parser.hpp>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

#include <Standard.hpp>

#ifdef WIN32
#define MiaExportModule(name) extern "C" inline __declspec(dllexport) mia::GeneratorModule* mia_exportModule() { static name instance; return &instance; }
#else
#define MiaExportModule(name) extern "C" inline mia::GeneratorModule* mia_exportModule() { static name instance; return &instance; }
#endif

namespace argumentum
{
	class ParameterConfig;
}

namespace mia
{
	struct DynamicLibrary;

	class CORE_EXPORT GeneratorConfig
	{
	public:
		int threadCount = -1;
		CppStandard cppStandard = CppStandard::Cpp11;
		bool dry = false;

		void registerOptions(argumentum::ParameterConfig& params);
	};

	class CORE_EXPORT GeneratorModule
	{
	public:
		using Ptr = std::shared_ptr<GeneratorModule>;
		using CreateFunc = GeneratorModule*(*)();

		virtual ~GeneratorModule() = default;

		virtual void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) = 0;
		[[nodiscard]]
		virtual const char* getVersion() const = 0;

		static GeneratorModule* loadFromLibrary(const DynamicLibrary& lib);
	};

	class GeneratorModuleBase
		: public GeneratorModule
	{
	public:
		GeneratorModuleBase() = default;
		~GeneratorModuleBase() override = default;

		[[nodiscard]]
		const char* getVersion() const override
		{
			return MIA_VERSION;
		}
	};

	class CORE_EXPORT Generator
	{
	public:
		Generator(const mia::GeneratorConfig& config, const std::vector<std::string>& includeDirs);
		
		void registerModule(GeneratorModule* module);
		void registerModules(const std::vector<GeneratorModule*>& modules);

		void generate(std::ostream& outputStream, const std::string& targetFile);
	private:
		static std::optional<cppast::cpp_standard> convertStandards(CppStandard standard);

		cppast::libclang_compile_config compileConfig;
		std::vector<GeneratorModule*> modules;
		cppast::libclang_parser parser;

		bool dry;
	};
}

#endif