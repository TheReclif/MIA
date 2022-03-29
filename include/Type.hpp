#pragma once
#ifndef MIA_TYPE_INCLUDED
#define MIA_TYPE_INCLUDED

#include "AttributableBase.hpp"

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <typeindex>

namespace mia
{
	namespace detail
	{
		template<class T>
		class Tag {};
	}

	enum class AccessSpecifier
	{
		Public,
		Protected,
		Private
	};

	struct Field
		: public Attributable
	{
		AccessSpecifier access;
		std::string_view name, type;

		inline Field(const AccessSpecifier spec, const char* name, const char* type, std::initializer_list<Attribute> l)
			: Attributable(std::move(l)), access(spec), name(name), type(type)
		{}
	};

	class Type
		: public Attributable
	{
	public:
		enum class Kind : unsigned char
		{
			Other,
			Struct,
			Class
		};
	public:
		Type() = default;
		template<class T>
		Type(detail::Tag<T>, std::string_view fullName, const Kind kind, std::vector<Field>&& f, std::initializer_list<Attribute> l, std::initializer_list<std::reference_wrapper<const Type>> parents)
			: Attributable(std::move(l)), fullyQualifiedName(fullName), kind(kind), typeIndex(typeid(T)), name(nameOf<T>()), bases(parents)
		{
			for (auto&& x : f)
			{
				const auto temp = x.name;
				fields.emplace(temp, std::move(x));
			}
			f.clear();
		}

		inline std::string_view getName() const
		{
			return name;
		}
		inline std::string_view getFullyQualifiedName() const
		{
			return fullyQualifiedName;
		}
		inline const std::type_index& getTypeIndex() const
		{
			return typeIndex;
		}
		inline const std::map<std::string_view, Field>& getFields() const
		{
			return fields;
		}
		inline const std::vector<std::reference_wrapper<const Type>>& getBases() const
		{
			return bases;
		}
	private:
		std::type_index typeIndex;
		std::string_view fullyQualifiedName, name;
		std::vector<std::reference_wrapper<const Type>> bases;
		std::map<std::string_view, Field> fields;
		Kind kind;
	};

	class TypeStorage
	{
	public:
		static inline TypeStorage& instance()
		{
			static TypeStorage storage;
			return storage;
		}

		inline void registerType(const Type& type)
		{
			const auto tid = type.getTypeIndex();
			const auto temp = types.emplace(tid, type);
			nameToTypes.emplace(temp.first->second.get().getFullyQualifiedName(), type);
		}

		template<class T>
		inline void unregisterType()
		{
			nameToTypes.erase(nameOf<T>());
			types.erase(typeid(T));
		}

		template<class T>
		inline const Type& getType() const
		{
			return types.at(typeid(T)).get();
		}

		inline const Type& getType(const std::string_view name) const
		{
			return nameToTypes.at(name).get();
		}

		inline const std::map<std::string_view, std::reference_wrapper<const Type>>& getTypes() const
		{
			return nameToTypes;
		}
	private:
		std::map<std::string_view, std::reference_wrapper<const Type>> nameToTypes;
		std::map<std::type_index, std::reference_wrapper<const Type>> types;
	};

	namespace detail
	{
		template<class T>
		class TypeRegistrator
		{
		public:
			TypeRegistrator()
			{
				TypeStorage::instance().registerType(typeOf<T>());
			}
			~TypeRegistrator()
			{
				TypeStorage::instance().unregisterType<T>();
			}
		};

		template<class Child>
		class AutoRegisterChild
		{
		private:
			static const TypeRegistrator<Child> myLovelyRegistrator;
		};
		template<class Child>
		const TypeRegistrator<Child> AutoRegisterChild<Child>::myLovelyRegistrator = TypeRegistrator<Child>();
	}
}

#endif
