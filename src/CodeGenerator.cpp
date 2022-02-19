#include <CodeGenerator.hpp>

cppast::code_generator::generation_options mia::CodeGenerator::do_get_options(const cppast::cpp_entity&, cppast::cpp_access_specifier_kind)
{
    return {};
}

void mia::CodeGenerator::do_indent()
{
    ++indentLevel;
}

void mia::CodeGenerator::do_unindent()
{
    --indentLevel;
}

void mia::CodeGenerator::do_write_token_seq(cppast::string_view tokens)
{
    // Append tokens
    str += tokens.c_str();
}

void mia::CodeGenerator::do_write_newline()
{
    str += '\n';
    str += std::string(indentLevel, '\t');
}

mia::CodeGenerator::CodeGenerator(const cppast::cpp_entity& e)
{
    // kickoff code generation here
    cppast::generate_code(*this, e);
}

const std::string& mia::CodeGenerator::code() const noexcept
{
    return str;
}
