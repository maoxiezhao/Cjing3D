#pragma once

#include <stdint.h>
#include <array>
#include <utility>

namespace Cjing3D {

	namespace ArrayImpl {

		/**
		*	\brief 返回填充重复数据后的std::array
		*/
		template<typename T, size_t... I>
		constexpr const auto FillArray(const T& value, std::index_sequence<I...>)
		{
			return std::array<T, sizeof...(I)>{
				(static_cast<void>(I), value)...		// ???? static_cast<void>(N), value
			};
		}

		/**
		*	\brief 扩充指定的Array
		*/
		template<typename T, size_t ToN, size_t... I>
		constexpr const auto Enlarge(const std::array<T, sizeof...(I)>& array, std::index_sequence<I...>)
		{
			return std::array<T, ToN>{ array[I]... };
		}

		template<typename T, size_t... I>
		constexpr const auto ArrayToTuple(const std::array<T, sizeof...(I)>& array, std::index_sequence<I...>)
		{
			return std::make_tuple(array[I]...);
		}

		template<typename T, typename TupleT, size_t...I>
		constexpr const auto TupleToArray(const TupleT& tuple, std::index_sequence<I...>)
		{
			return std::array<T, sizeof...(I)>{ std::get<I>(tuple)... };
		}
	}

	// 以相同数据填充Array
	template< typename T, size_t N>
	constexpr const auto FillArray(const T& value)
	{
		return ArrayImpl::FillArray(value, std::make_index_sequence<N>());
	}

	// 扩充Enlarge
	template <typename T, size_t ToN, size_t FromN>
	constexpr const auto EnlargeArray(const std::array<T, FromN>& array)
	{
		return ArrayImpl::Enlarge<T, ToN>(array, std::make_index_sequence<FromN>());
	}

	template <typename T, size_t N>
	constexpr const auto ArrayToTuple(const std::array<T, N>& array)
	{
		return ArrayImpl::ArrayToTuple(array, std::make_index_sequence<N>());
	}


	template<typename T, typename... Ts>
	constexpr const auto TupleToArray(const std::tuple<T, Ts...>& t)
	{
		constexpr auto N = sizeof...(Ts) + 1;
		return ArrayImpl::TupleToArray<T>(t, std::make_index_sequence<N>());
	}

	/**
	*	\brief Array 提供了局部到全部及全部到局部的支持
	*/
	template<typename T, size_t N, size_t AlignT = alignof(T), typename = std::enable_if_t< (N != 1) >>
	class Array : public std::array<T, N>
	{
	public:
		constexpr Array()noexcept : std::array<T, N>{} {};
		constexpr Array(const T& value)noexcept :
			std::array<T, N>(FillArray<T, N>(value)) {}

		// 当传入的args个数等于N时
		template<typename... Args, typename = std::enable_if_t< (N == sizeof...(Args))> >
		constexpr Array(Args&&... args) noexcept :
			std::array<T, N>{ std::forward<Args>(args)...} {}

		// 当传入的array的FromN小于N时
		template<size_t	FromN, typename = std::enable_if_t< (FromN < N)> >
			constexpr Array(const Array<T, FromN, AlignT>& other) noexcept :
				std::array<T, N>(EnlargeArray<T, N>(other)) {}


		// 当传入的array的FromN小于N时, 且存在额外参数,总参数个数等于N时,先
		// 转换为tuple连接后再转换为array
		template< size_t FromN, typename... Args,
			typename = std::enable_if_t< (FromN < N && (FromN + sizeof...(Args) == N)) > >
			constexpr Array(const Array<T, FromN, AlignT>& other, Args&&... args) noexcept :
			std::array<T, N>(TupleToArray(std::tuple_cat(ArrayToTuple(other),
				std::make_tuple(std::forward<Args>(args)...)))) {}

			~Array() = default;

		// not fast, only use in simple case.
		template<typename T, size_t N>
		std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, Array<T, N>> 
			operator+(const Array<T, N>& lhs)const
		{
			Array<T, N> result;
			for (int i = 0; i < N; i++) {
				result[i] = lhs[i] + this->at(i);
			}
			return result;
		}
		template<typename T, size_t N>
		std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, Array<T, N>> 
			operator-(const Array<T, N>& lhs)const
		{
			Array<T, N> result;
			for (int i = 0; i < N; i++) {
				result[i] = this->at(i) - lhs[i];
			}
			return result;
		}
		template<typename T, size_t N>
		std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, Array<T, N>>& 
			operator+=(const Array<T, N>& lhs)
		{
			for (int i = 0; i < N; i++) {
				this->_Elems[i] += lhs[i];
			}
			return *this;
		}
		template<typename T, size_t N>
		std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, Array<T, N>>& 
			operator-=(const Array<T, N>& lhs)
		{
			for (int i = 0; i < N; i++) {
				this->_Elems[i] -= lhs[i];
			}
			return *this;
		}
	};

}

// 用于支持结构体绑定声明
namespace std {
	template<typename T, size_t N, size_t A>
	struct tuple_size<Cjing3D::Array<T, N, A> > : public integral_constant<size_t, N> {};

	template<size_t I, typename T, size_t N, size_t A>
	struct tuple_element<I, Cjing3D::Array<T, N, A> > {
		using type = T;
	};
}