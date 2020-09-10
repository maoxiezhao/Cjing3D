// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.
#pragma once

#include <memory>
#include <utility>
#include <type_traits>

namespace Cjing3D
{
	namespace EventImpl
	{
		template<typename T>
		constexpr void CheckEventType()
		{
			static_assert(!std::is_reference<T>::value, "reference type is not supported.");
			static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");
			static_assert(std::is_object<T>::value, "T is object type.");
		}

		template <class T>
		struct EventHashCode final {
			static const std::size_t value;
		};

		template <class T>
		const std::size_t EventHashCode<T>::value = typeid(const T*).hash_code();

		class EventBody {
		public:
			EventBody() = default;
			EventBody(const EventBody&) = delete;
			EventBody& operator=(const EventBody&) = delete;
			virtual ~EventBody() = default;

			virtual std::size_t HashCode() const noexcept = 0;
		};

		template <typename T>
		class EventBodyOverride final : public EventBody
		{
		public:
			T mData;

		public:
			template <typename... Args>
			explicit EventBodyOverride(Args&&... argument)
			{
				CheckEventType<T>();
				mData = T(std::forward<Args>(argument)...);
			}

			std::size_t HashCode() const noexcept override
			{
				return EventHashCode<T>::value;
			}
		};
	}

	class Event final
	{
	private:
		std::unique_ptr<EventImpl::EventBody> mEventBody = nullptr;

	public:
		Event() = delete;
		Event(const Event&) = delete;
		Event(Event&&) = default;
		Event& operator=(const Event&) = delete;
		Event& operator=(Event&&) = default;

		template<typename T>
		explicit Event(T&& value)
		{
			typedef typename std::remove_reference<T>::type ValueType;
			typedef EventImpl::EventBodyOverride<ValueType> BodyType;

			EventImpl::CheckEventType<ValueType>();

			mEventBody = std::make_unique<BodyType>(std::forward<T>(value));
		}

		template<typename T>
		bool Is()const
		{
			EventImpl::CheckEventType<T>();
			return mEventBody != nullptr && (mEventBody->HashCode() == EventImpl::EventHashCode<T>::value);
		}

		template<typename T>
		const T* As()const
		{
			typedef EventImpl::EventBodyOverride<T> BodyType;
			EventImpl::CheckEventType<T>();
			
			auto bodyPtr = dynamic_cast<const BodyType*>(mEventBody.get());
			if (bodyPtr == nullptr) {
				return nullptr;
			}

			return &bodyPtr->mData;
		}
	};
}