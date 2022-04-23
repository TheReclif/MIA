#pragma once
#ifndef MIA_TYPE_INCLUDED
#define MIA_TYPE_INCLUDED

#include "AttributableBase.hpp"

#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <typeindex>
#include <functional>

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

	struct FieldTypeMismatchException
		: public std::exception
	{
		virtual const char* what() const noexcept override
		{
			return "Type mismatch between passed type and field/object type";
		}
	};

	class Field
		: public Attributable
	{
	public:
		inline Field(const AccessSpecifier spec, const char* name, const char* type, std::initializer_list<Attribute> l, std::type_index&& cType, std::type_index&& fType)
			: Attributable(std::move(l)), access(spec), name(name), type(type), classType(std::move(cType)), fieldType(std::move(fType))
		{
		}

		template<class T1, class T2>
		const T2* get(const T1* const obj)
		{
			if (typeid(T1) != classType || typeid(T2) != fieldType)
			{
				throw FieldTypeMismatchException();
			}

			return static_cast<const void*>(getter(static_cast<const void*>(obj)));
		}

		template<class T1, class T2>
		void set(T1* const obj, const T2* const arg)
		{
			if (typeid(T1) != classType || typeid(T2) != fieldType)
			{
				throw FieldTypeMismatchException();
			}

			getter(static_cast<const void*>(obj), static_cast<void*>(arg));
		}
	private:
		AccessSpecifier access;
		std::string_view name, type;
		std::function<void(void*, const void*)> setter;
		std::function<const void*(const void*)> getter;
		std::type_index classType, fieldType;
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
		Type(const detail::Tag<T>, const std::string_view fullName, const Kind kind, std::vector<Field>&& f, const std::initializer_list<Attribute> l, const std::initializer_list<std::reference_wrapper<const Type>> parents)
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
		public:
			static const TypeRegistrator<Child> myLovelyRegistrator;
		public:
			AutoRegisterChild()
			{
				(void)myLovelyRegistrator;
			}
		};
		template<class Child>
		const TypeRegistrator<Child> AutoRegisterChild<Child>::myLovelyRegistrator = TypeRegistrator<Child>();
	}
}

#endif
