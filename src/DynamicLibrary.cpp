#include "DynamicLibrary.hpp"

#include <string>

#ifdef WIN32
#include <windows.h>

class WindowsDynamicLibrary
	: public mia::DynamicLibrary
{
	HMODULE handle = nullptr;
	std::string dllName;
public:
	virtual ~WindowsDynamicLibrary()
	{
		unload();
	}

	virtual void load(const char* name) override
	{
		unload();
		
		handle = LoadLibraryA(name);
		dllName = name;
	}

	virtual DummyFuncPtr getFuncAddress(const char* name) const override
	{
		if (!handle)
		{
			return nullptr;
		}

		return reinterpret_cast<DummyFuncPtr>(GetProcAddress(handle, name));
	}

	virtual std::string_view getName() const override
	{
		return dllName;
	}

	void unload()
	{
		if (handle)
		{
			FreeLibrary(handle);
			handle = nullptr;
			dllName.clear();
		}
	}
};

using DynamicLibraryImpl = WindowsDynamicLibrary;
#endif

std::unique_ptr<mia::DynamicLibrary> mia::DynamicLibraryFactory::create()
{
	return std::make_unique<DynamicLibraryImpl>();
}
