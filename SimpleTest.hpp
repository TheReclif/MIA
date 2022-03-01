#pragma once

namespace GLOWE
{
	enum class [[mia::include]] ETest
	{
		Default,
		UInt16 [[mia::name("Unsigned short")]],
		UInt32 [[mia::name("Unsigned int")]]
	};
}
