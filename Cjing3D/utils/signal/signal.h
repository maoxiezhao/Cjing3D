// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.
#pragma once

#include "utils\signal\signalInclude.h"
#include "utils\signal\connection.h"

namespace Cjing3D
{

	namespace SingalImpl
	{
		template<typename... Args>
		class SignalBody<void(Args...)> final : public std::enable_shared_from_this<SignalBody<void(Args...)>>
		{
			using CallbackFunc = std::function<void(Args...)>;
			using ConnectionType = ConnectionBodyOverride<void(Args...)>;
		public:
			SignalBody() = default;
			~SignalBody()
			{
				mObservers.clear();
				mAddedObservers.clear();
			}

			SignalBody(const SignalBody& rhs) = delete;
			SignalBody& operator=(const SignalBody& rhs) = delete;
			SignalBody(SignalBody&& rhs) = delete;
			SignalBody& operator=(SignalBody&& rhs) = delete;

			std::unique_ptr<ConnectionType> Connect(CallbackFunc&& func)
			{
				U32 slotIndex = 0;
				{
					GuardSpinLock lock(mAddSpinLock);

					slotIndex = mNextSlotIndex;
					++mNextSlotIndex;

					Debug::CheckAssertion(
						std::end(mAddedObservers) == std::find_if(
							std::begin(mAddedObservers),
							std::end(mAddedObservers),
							[&](const auto& pair) { return pair.first == slotIndex; })
					);
					mAddedObservers.emplace_back(slotIndex, std::forward<CallbackFunc>(func));
				}

				std::weak_ptr<SignalBody> weakSignal = this->shared_from_this();
				return std::make_unique<ConnectionType>(std::move(weakSignal), slotIndex);
			}

			void Disconnect(U32 index);
			void Call(Args&&... args);
			bool IsConnected(U32 index);

		private:
			void PushBackAddedListeners();
			void EraseRemovedListeners();

			std::vector <std::pair<U32, CallbackFunc>> mObservers;
			std::vector <std::pair<U32, CallbackFunc>> mAddedObservers;

			SpinLock mAddSpinLock;
			SpinLock mSlotSpinLock;

			U32 mNestedMethodCallCount = 0;
			U32 mNextSlotIndex = 0;
			const U32 MaxNestedMethodCallCount = 128;
		};

		template<typename ...Args>
		inline void SignalBody<void(Args...)>::Disconnect(U32 index)
		{
			{
				GuardSpinLock lock(mAddSpinLock);
				auto it = std::find_if(
					std::begin(mAddedObservers),
					std::end(mAddedObservers),
					[&](const auto& pair) { return pair.first == index; });
				if (it != std::end(mAddedObservers)) {
					mAddedObservers.erase(it);
				}
			}
			{
				GuardSpinLock lock(mSlotSpinLock);
				auto it = std::find_if(mObservers.begin(), mObservers.end(),
					[&](const auto& pair) {
						return pair.first == index;
					});

				if (it != mObservers.end()) {
					it->second = nullptr;
				}
			}
		}

		template<typename ...Args>
		inline void SignalBody<void(Args...)>::Call(Args&& ...args)
		{
			if (mNestedMethodCallCount <= 0) {
				PushBackAddedListeners();
			}

			if (mNestedMethodCallCount >= MaxNestedMethodCallCount) {
				return;
			}

			Debug::CheckAssertion(mNestedMethodCallCount >= 0);
			++mNestedMethodCallCount;

			try
			{
				for (auto& pair : mObservers)
				{
					if (auto f = pair.second; f != nullptr)
					{
						f(std::forward<Args>(args)...);
					}
				}
			}
			catch (const std::exception & e)
			{
				--mNestedMethodCallCount;
				throw e;
			}

			Debug::CheckAssertion(mNestedMethodCallCount > 0);
			--mNestedMethodCallCount;

			if (mNestedMethodCallCount <= 0) {
				EraseRemovedListeners();
			}
		}

		template<typename ...Args>
		inline bool SignalBody<void(Args...)>::IsConnected(U32 index)
		{
			{
				GuardSpinLock lock(mSlotSpinLock);
				auto it = std::find_if(mObservers.begin(), mObservers.end(), [&](const auto& pair) {
					return pair.first == index;
					});

				if (it != mObservers.end()) {
					return it->second != nullptr;
				}
			}

			GuardSpinLock lock(mAddSpinLock);
			auto iter = std::find_if(
				std::begin(mAddedObservers),
				std::end(mAddedObservers),
				[&](const auto& pair) { return pair.first == index; });

			return (iter != std::end(mAddedObservers)) && (iter->second != nullptr);
		}

		template<typename ...Args>
		inline void SignalBody<void(Args...)>::PushBackAddedListeners()
		{
			decltype(mAddedObservers) temporarySlots;
			{
				GuardSpinLock lock(mAddSpinLock);
				std::swap(temporarySlots, mAddedObservers);
			}
			{
				GuardSpinLock lock(mSlotSpinLock);
				for (auto& slot : temporarySlots)
				{
					auto it = std::find_if(std::begin(mObservers), std::end(mObservers),
						[&](const auto& pair) { return pair.first == slot.first; });

					if (it == mObservers.end()) {
						mObservers.push_back(std::move(slot));
					}
				}
			}
		}

		template<typename ...Args>
		inline void SignalBody<void(Args...)>::EraseRemovedListeners()
		{
			GuardSpinLock lock(mSlotSpinLock);
			auto it = std::remove_if(std::begin(mObservers), std::end(mObservers),
				[](const auto& pair) { return pair.second == nullptr; });
			mObservers.erase(it, std::end(mObservers));
		}
	}

	// 基于监听者模式实现的signal
	template<typename... Args>
	class Signal<void(Args...)> final
	{
		using SignalBody = SingalImpl::SignalBody<void(Args...)>;
	public:
		Signal() : mSignalBody(std::make_shared<SignalBody>()) {}

		Signal(const Signal& rhs) = delete;
		Signal& operator=(const Signal& rhs) = delete;
		Signal(Signal&& rhs) = default;
		Signal& operator=(Signal&& rhs) = default;

		Connection Connect(const std::function<void(Args...)>& func)
		{
			return Connection(std::move(mSignalBody->Connect(func)));
		}

		Connection Connect(std::function<void(Args...)>&& func)
		{
			return Connection(mSignalBody->Connect(std::move(func)));
		}

		void operator()(Args&&... args)
		{
			mSignalBody->Call(std::forward<Args>(args)...);
		}
	private:	
		std::shared_ptr<SignalBody> mSignalBody ;
	};
}