#ifndef MIAUTILS_INCLUDED
#define MIAUTILS_INCLUDED
#include <string>
#include <unordered_map>
#include <stdexcept>

#include <Type.hpp>

template<typename T>
class EnumConverter {};
namespace detail
{
	template<class T>
	struct NameOf
	{
		static const char* name()
		{
			return typeid(T).name();
		}
	};
	template<class T>
	struct TypeOf
	{
		static const mia::Type& type()
		{
			static mia::Type type(mia::detail::Tag<T>(), nameOf<T>(), mia::Type::Kind::Other, {}, {}, {});
			return type;
		}
	};
}

namespace mia
{
	struct Serializable;

	struct Referencable
	{
		virtual ~Referencable() = default;
	};

	struct Serializer
	{
		virtual ~Serializer() = default;

		virtual void serialize(const std::string_view name, std::string&& value) = 0;
		virtual void serialize(const std::string_view name, const Serializable& value) = 0;
		virtual void serialize(const std::string_view name, const Referencable* const ref) = 0;

		virtual const std::string& deserialize(const std::string_view name) = 0;
		virtual void deserialize(const std::string_view name, Serializable& value) = 0;
		virtual Referencable* deserializeRef(const std::string_view name) = 0;
	};

	struct Serializable
	{
		virtual ~Serializable() = default;

		virtual void serialize(std::ostream&) const = 0;
		virtual void deserialize(std::istream&) = 0;
	};
}

template<class T>
constexpr const char* nameOf()
{
	return detail::NameOf<T>::name();
}

template<class T>
constexpr const char* nameOf(const T&)
{
	return nameOf<T>();
}

template<class T>
const mia::Type& typeOf()
{
	return detail::TypeOf<T>::type();
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

template<class T> void serialize(std::ostream& out, const T& arg)
{
	out << arg;
}

template<class T> void deserialize(std::istream& in, T& arg)
{
	in >> arg;
}

template<> inline void serialize(std::ostream& out, const mia::Serializable& arg)
{
	arg.serialize(out);
}

template<> inline void deserialize(std::istream& in, mia::Serializable& arg)
{
	arg.deserialize(in);
}
#endif
