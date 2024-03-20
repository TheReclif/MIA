#include <Generator.hpp>

#include <cppast/visitor.hpp>

class TestModule
	: public mia::GeneratorModule
{
	void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) override
	{
		cppast::visit(source, [](const cppast::cpp_entity& e, cppast::visitor_info info) -> bool
			{
				std::cout << "[dynamic_module] " << e.name() << std::endl;

				return true;
			});
	}
};

MiaExportModule(TestModule)
