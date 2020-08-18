#pragma once

#include "common\common.h"
#include "helper\stringID.h"
#include "helper\enumInfo.h"
#include "utils\math.h"

#include <variant>
#include <type_traits>
#include <vector>
#include <map>

namespace Cjing3D {

class JsonArchive;

#define _VARIANT_TYPES				\
	std::monostate,                 \
	char,							\
	unsigned char,					\
	int,							\
	unsigned int,					\
	bool,							\
	float,							\
	double,							\
    std::string,                    \
	void*,							\
    I32x2,							\
    I32x3,							\
    I32x4							

	using VariantType = std::variant<_VARIANT_TYPES>;

	/**
	*	\brief the class of Variant
	*/
	class Variant
	{
	public:
		Variant(){}		
		template<typename T, typename = std::enable_if_t<!std::is_same<T, Variant>::value> >
		Variant(T value) 
		{
			mVariant = value;
			mType = MappingType<T>();
		}
		Variant(const Variant& var) 
		{
			mVariant = var.GetVariant();
			mType = var.GetType();
		}
		Variant& operator=(const Variant& var) 
		{
			mVariant = var.GetVariant();
			mType = var.GetType();
			return *this;
		}
		Variant(Variant&& var) = default;
		Variant& operator=(Variant&& var) = default;
		~Variant() = default;

		const VariantType& GetVariant()const {
			return mVariant;
		}

		template <typename T>
		const T& GetValue()const {
			return std::get<T>(mVariant);
		}

		template <typename T>
		void SetValue(T&& value) {
			mVariant = value;
		}

		bool IsEmpty()const {
			return mType == TYPE_UNKNOW;
		}

	public:
		enum Type
		{
			TYPE_UNKNOW,
			TYPE_CHAR,
			TYPE_UNSIGNED_CHAR,
			TYPE_INT,
			TYPE_UNSIGNED_INT,
			TYPE_BOOL,
			TYPE_FLOAT,
			TYPE_DOUBLE,
			TYPE_STRING,
			TYPE_VOID_PTR,
			TYPE_I32X2,
			TYPE_I32X3,
			TYPE_I32X4
		};
		template <typename ResourceT>
		Type MappingType();

		Type GetType()const {
			return mType;
		}

		void Serialize(JsonArchive& archive);
		void Unserialize(JsonArchive& archive)const;

	private:
		VariantType mVariant;
		Type mType = TYPE_UNKNOW;
	};

	using VariantArray = std::vector<Variant>;
	using VariantMap = std::map<StringID, Variant>;

	ENUM_TRAITS_REGISTER_ENUM_HEADER(Variant::Type)

#include "variant.inl"
}