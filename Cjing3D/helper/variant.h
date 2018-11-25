#pragma once

#include "common\common.h"
#include "helper\stringID.h"

#include <variant>
#include <type_traits>
#include <vector>
#include <map>

namespace Cjing3D {

class Actor;

#define _VARIANT_TYPES				\
	char,							\
	unsigned char,					\
	int,							\
	unsigned int,					\
	bool,							\
	float,							\
	double,							\
	void*											

	using VariantType = std::variant<_VARIANT_TYPES>;

	/**
	*	\brief the class of Variant
	*/
	class Variant
	{
	public:
		template<typename T, typename = std::enable_if_t<!std::is_same<T, Variant>::value> >
		Variant(T value) {
			mVariant = value;
		}

		Variant(const Variant& var) {
			mVariant = var.GetVariant();
		}

		Variant() {};
		Variant(Variant&& var) = default;

		Variant& operator=(const Variant& var) {
			mVariant = var.GetVariant();
		}

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

	private:
		VariantType mVariant;
	};

	using VariantArray = std::vector<Variant>;
	using VariantMap = std::map<StringID, Variant>;
}