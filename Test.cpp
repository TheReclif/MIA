#include "out/build/x64-Debug/SimpleTest.mia.hpp"

#include <iostream>

int main()
{
	GLOWE::TemplatedTest<float> t;
	GLOWE::TemplatedTest<int> xd;

	const auto& types = mia::TypeStorage::instance();
	for (const auto& type : types.getTypes())
	{
		std::cout << "type " << type.first << std::endl;
		for (const auto& field : type.second.get().getFields())
		{
			std::cout << "\tfield " << field.second.type << ' ' << field.first << '\n';
		}
		for (const auto& base : type.second.get().getBases())
		{
			std::cout << "\tbase class " << base.get().getFullyQualifiedName() << '\n';
		}
	}
	
	return 0;
}
