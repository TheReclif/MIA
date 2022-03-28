#include "out/build/x64-Debug/SimpleTest.mia.hpp"

#include <iostream>

int main()
{
	std::cout << typeOf<GLOWE::CTest>().getFullyQualifiedName() << std::endl;
	const auto& type = typeOf<GLOWE::CTest>();
	for (const auto& field : type.getFields())
	{
		std::cout << "\t" << field.second.type << " " << field.second.name << std::endl;
	}
	
	return 0;
}
