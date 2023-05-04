#pragma once
#ifndef CODEGENERATOR_INCLUDED
#define CODEGENERATOR_INCLUDED

#include <core_export.h>

#include <cppast/code_generator.hpp>         // for generate_code()
#include <cppast/cpp_entity.hpp>

namespace mia
{
	class CORE_EXPORT CodeGenerator
		: public cppast::code_generator
	{
    private:
        std::string str;
        int indentLevel = 0;
    private:
        // Called to retrieve the generation options of an entity
        generation_options do_get_options(const cppast::cpp_entity&,
            cppast::cpp_access_specifier_kind) override;

        void do_indent() override;
        void do_unindent() override;

        // Called when a generic token sequence should be generated
        // there are specialized callbacks for various token kinds,
        // to e.g. implement syntax highlighting
        void do_write_token_seq(cppast::string_view tokens) override;
        void do_write_newline() override;
    public:
        CodeGenerator(const cppast::cpp_entity& e);

        // return the result
        const std::string& code() const noexcept;
	};
}

#endif
