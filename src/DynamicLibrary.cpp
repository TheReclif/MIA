#include "DynamicLibrary.hpp"

#include <string>

#ifdef WIN32
#include <windows.h>

class WindowsDynamicLibrary
	: public mia::DynamicLibrary
{
	HMODULE handle = nullptr;
public:
	virtual ~WindowsDynamicLibrary()
	{
		unload();
	}

	virtual void load(const char* name) override
	{
		unload();
		
		handle = LoadLibraryA(name);
	}

	virtual DummyFuncPtr getFuncAddress(const char* name) const override
	{
		if (!handle)
		{
			return nullptr;
		}

		return reinterpret_cast<DummyFuncPtr>(GetProcAddress(handle, name));
	}

	void unload()
	{
		if (handle)
		{
			FreeLibrary(handle);
			handle = nullptr;
		}
	}
};

using DynamicLibraryImpl = WindowsDynamicLibrary;
#endif

std::unique_ptr<mia::DynamicLibrary> mia::DynamicLibraryFactory::create()
{
	return std::make_unique<DynamicLibraryImpl>();
}
