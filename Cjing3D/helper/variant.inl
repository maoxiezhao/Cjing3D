template<>
inline Variant::Type Variant::MappingType<char>()
{
	return TYPE_CHAR;
}
template<>
inline Variant::Type Variant::MappingType<unsigned char>()
{
	return TYPE_UNSIGNED_CHAR;
}
template<>
inline Variant::Type Variant::MappingType<int>()
{
	return TYPE_INT;
}
template<>
inline Variant::Type Variant::MappingType<unsigned int>()
{
	return TYPE_UNSIGNED_INT;
}
template<>
inline Variant::Type Variant::MappingType<bool>()
{
	return TYPE_BOOL;
}
template<>
inline Variant::Type Variant::MappingType<float>()
{
	return TYPE_FLOAT;
}
template<>
inline Variant::Type Variant::MappingType<double>()
{
	return TYPE_FLOAT;
}
template<>
inline Variant::Type Variant::MappingType<std::string>()
{
	return TYPE_STRING;
}
template<>
inline Variant::Type Variant::MappingType<void*>()
{
	return TYPE_VOID_PTR;
}
template<>
inline Variant::Type Variant::MappingType<I32x2>()
{
	return TYPE_I32X2;
}
template<>
inline Variant::Type Variant::MappingType<I32x3>()
{
	return TYPE_I32X3;
}
template<>
inline Variant::Type Variant::MappingType<I32x4>()
{
	return TYPE_I32X4;
}