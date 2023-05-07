#include <SimpleTest.hpp>
#include <SimpleTest.mia.hpp>

#include <iostream>

int main()
{
	GLOWE::TemplatedTest<int> xd;

	const auto& types = mia::TypeStorage::instance();
	for (const auto& type : types.getTypes())
	{
		std::cout << "type " << type.first << std::endl;
		for (const auto& field : type.second.get().getFields())
		{
			std::cout << "\tfield " << field.second.getTypeName() << ' ' << field.first << '\n';
		}
		for (const auto& base : type.second.get().getBases())
		{
			std::cout << "\tbase class " << base.get().getFullyQualifiedName() << '\n';
		}
	}

	GLOWE::Test t;
	t.xxx = 3.0f;
	float newVal = 8.0f;
	typeOf(t).getFields().at("xxx").set(&t, &newVal);
	std::cout << "Legit way: " << t.xxx << std::endl;
	std::cout << "Hackish way: " << *typeOf(t).getFields().at("xxx").get<decltype(t), float>(&t) << std::endl;
	
	return 0;
}
