#pragma once

namespace GLOWE
{
	enum class [[mia_gen::enum_string]] ETest
	{
		Default,
		UInt16 [[mia_gen::enum_val_name("Unsigned short")]],
		UInt32 [[mia_gen::enum_val_name("Unsigned int")]]
	};
}
