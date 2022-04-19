#pragma once

#include <MiaUtils.hpp>

template<class T>
void registerThis(const T&)
{
	mia::detail::AutoRegisterChild<T>().myLovelyRegistrator;
}

namespace GLOWE
{
	enum class [[mia::include]] ETest
	{
		Default,
		UInt16 [[mia::name("Unsigned short")]],
		UInt32 [[mia::name("Unsigned int")]]
	};

	class Test
	{
	public:
		float xxx;
	};

	class [[mia::serialize]] Test2
		: public Test
	{
		[[mia::include(true)]]
		float a = 8.0f;
	public:
		int b;
		int* ptr;
	protected:
		int arr[8];
	public:
		[[u::ServerRpc]]
		inline void serverRpc()
		{
			int xaz = 7;
		}
	};

	template<class T>
	class [[mia::serialize]] TemplatedTest
		: public mia::detail::AutoRegisterChild<TemplatedTest<T>>
	{
	public:
		[[dupaaaaa]]
		T xxxxxx;
	public:
		TemplatedTest()
			: xxxxxx{}
		{
		}
	};
}
