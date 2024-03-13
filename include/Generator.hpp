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
#define MiaExportModule(name) extern "C" inline __declspec(dllexport) mia::GeneratorModule* mia_exportModule(const char* ver) { static name instance; return std::string_view(ver) == std::string_view(MIA_VERSION) ? &instance : nullptr; } \
extern "C" inline __declspec(dllexport) const char* mia_getVersion() { return MIA_VERSION; }
#else
#define MiaExportModule(name) extern "C" inline mia::GeneratorModule* mia_exportModule() { static name instance; return std::string_view(ver) == std::string_view(MIA_VERSION) ? &instance : nullptr; }
extern "C" inline const char* mia_getVersion() { return MIA_VERSION; }
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
		int threadCount = 0;
		CppStandard cppStandard = CppStandard::Cpp11;
		bool dry = false;
		bool verbose = false;
		bool textOutput = false;

		void registerOptions(argumentum::ParameterConfig& params);
		void log() const;
	};

	class CORE_EXPORT VersionError: public std::runtime_error
	{
	public:
		VersionError(const char* version);
	};

	class CORE_EXPORT LoadError : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	class CORE_EXPORT GeneratorModule
	{
	public:
		using Ptr = std::shared_ptr<GeneratorModule>;
		using CreateFunc = GeneratorModule*(*)(const char*);
		using GetVersionFunc = const char*(*)();

		virtual ~GeneratorModule() = default;

		virtual void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) = 0;

		static GeneratorModule* loadFromLibrary(const DynamicLibrary& lib);
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

		const bool dry;
		const bool verbose;
	};
}

#endif