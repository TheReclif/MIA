#pragma once

namespace GLOWE
{
	enum class [[mia::include]] ETest
	{
		Default,
		UInt16 [[mia::name("Unsigned short")]],
		UInt32 [[mia::name("Unsigned int")]]
	};

	class CTest
	{
	public:
		float xxx;
	};

	class CTest2
		: public CTest
	{
		float a;
	public:
		int b;
		int* ptr;
	protected:
		int arr[8];
	};
}
