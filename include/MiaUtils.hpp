#ifndef MIAUTILS_INCLUDED
#define MIAUTILS_INCLUDED
#include <string>
#include <unordered_map>
#include <stdexcept>

#include <Type.hpp>

template<typename T> class EnumConverter {};

template<class T>
constexpr const char* nameOf()
{
	return typeid(T).name();
}

template<class T>
constexpr const char* nameOf(const T&)
{
	return nameOf<T>();
}

template<class T>
const mia::Type& typeOf()
{
	static mia::Type type(mia::detail::Tag<T>(), nameOf<T>(), mia::Type::Kind::Other, {}, {});
	return type;
}

template<class T>
mia::Type typeOf(const T&)
{
	return typeOf<T>();
}

template<typename T> T to_enum(const std::string&)
{
	throw std::logic_error("Enum conversion for given type not implemented.");
}

template<class T> void serialize(std::ostream&, const T&)
{
	throw std::logic_error("Serialization for " + std::string(nameOf<T>()) + " not implemented.");
}

template<class T> void deserialize(std::istream&, const T&)
{
	throw std::logic_error("Serialization for " + std::string(nameOf<T>()) + " not implemented.");
}
#endif
