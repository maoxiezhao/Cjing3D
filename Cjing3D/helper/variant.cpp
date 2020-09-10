#include "variant.h"
#include "jsonArchive.h"

namespace Cjing3D 
{

ENUM_TRAITS_REGISTER_ENUM_BEGIN(Variant::Type)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_UNKNOW)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_CHAR)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_UNSIGNED_CHAR)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_INT)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_UNSIGNED_INT)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_BOOL)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_FLOAT)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_DOUBLE)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_STRING)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_VOID_PTR)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_I32X2)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_I32X3)
ENUM_TRAITS_REGISTER_ENUM_DEFINE(Variant::TYPE_I32X4)
ENUM_TRAITS_REGISTER_ENUM_END(Variant::Type)


void Variant::Serialize(JsonArchive& archive)
{
	std::string typeString;
	archive.Read<std::string>("type", typeString);
	mType = StringToEnum<Variant::Type>(typeString);

	switch (mType)
	{
	case Variant::TYPE_CHAR:
	{
		char value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_UNSIGNED_CHAR:
	{
		unsigned char value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_INT:
	{
		int value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_UNSIGNED_INT:
	{
		unsigned int value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_BOOL:
	{
		bool value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_FLOAT:
	{
		F32 value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_DOUBLE:
	{
		F64 value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_STRING:
	{
		std::string value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_I32X2:
	{
		I32x2 value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_I32X3:
	{
		I32x3 value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	case Variant::TYPE_I32X4:
	{
		I32x4 value;
		archive.Read("val", value);
		SetValue(value);
	}
		break;
	default:
		mType = TYPE_UNKNOW;
		break;
	}
}

void Variant::Unserialize(JsonArchive& archive)const
{
	// type
	archive.Write("type", EnumToString(GetType()));

	// value
	switch (mType)
	{
	case Variant::TYPE_CHAR:
		archive.Write("val", GetValue<char>());
		break;
	case Variant::TYPE_UNSIGNED_CHAR:
		archive.Write("val", GetValue<unsigned char>());
		break;
	case Variant::TYPE_INT:
		archive.Write("val", GetValue<int>());
		break;
	case Variant::TYPE_UNSIGNED_INT:
		archive.Write("val", GetValue<unsigned int>());
		break;
	case Variant::TYPE_BOOL:
		archive.Write("val", GetValue<bool>());
		break;
	case Variant::TYPE_FLOAT:
		archive.Write("val", GetValue<F32>());
		break;
	case Variant::TYPE_DOUBLE:
		archive.Write("val", GetValue<F64>());
		break;
	case Variant::TYPE_STRING:
		archive.Write("val", GetValue<std::string>());
		break;
	case Variant::TYPE_I32X2:
		archive.Write("val", GetValue<I32x2>());
		break;
	case Variant::TYPE_I32X3:
		archive.Write("val", GetValue<I32x3>());
		break;
	case Variant::TYPE_I32X4:
		archive.Write("val", GetValue<I32x4>());
		break;
	default:
		break;
	}
}


}