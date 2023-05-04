#include <Generator.hpp>

#include <cppast/visitor.hpp>

class TestModule
	: public mia::GeneratorModule
{
	const char* getVersion() const override
	{
		return MIA_VERSION;
	}

	void extractInfo(std::ostream& outputStream, cppast::cpp_file& source) override
	{
		cppast::visit(source, [](const cppast::cpp_entity& e, cppast::visitor_info info) -> bool
			{
				std::cout << e.name() << std::endl;

				return true;
			});
	}
};
