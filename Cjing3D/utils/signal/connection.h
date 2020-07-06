// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.
#pragma once

#include "utils\signal\signalInclude.h"

namespace Cjing3D
{
	namespace SingalImpl
	{
		class ConnectionBody
		{
		public:
			virtual ~ConnectionBody() = default;
			virtual void Disconnect() = 0;
			[[nodiscard]] virtual bool Valid() const = 0;
			[[nodiscard]] virtual std::unique_ptr<ConnectionBody> DeepCopy() const = 0;
		};


		template <typename Function>
		class ConnectionBodyOverride final : public ConnectionBody
		{
			using WeakSignal = std::weak_ptr<SignalBody<Function>>;
		public:
			ConnectionBodyOverride() = default;
			ConnectionBodyOverride(const ConnectionBodyOverride&) = delete;
			ConnectionBodyOverride& operator=(const ConnectionBodyOverride&) = delete;

			ConnectionBodyOverride(WeakSignal&& weakSignalIn, U32 slotIndexIn)
				: mWeakSignal(std::forward<WeakSignal>(weakSignalIn))
				, mSlotIndex(slotIndexIn)
			{
			}

			void Disconnect() override
			{
				if (mSlotIndex == std::nullopt) {
					return;
				}

				if (auto lockedSignal = mWeakSignal.lock(); lockedSignal != nullptr)
				{
					lockedSignal->Disconnect(*mSlotIndex);
					mWeakSignal.reset();
				}
				mSlotIndex = std::nullopt;
			}

			[[nodiscard]] bool Valid() const override
			{
				if (mSlotIndex == std::nullopt) {
					return false;
				}

				if (auto lockedSignal = mWeakSignal.lock(); lockedSignal != nullptr) {
					return lockedSignal->IsConnected(*mSlotIndex);
				}
				return false;
			}

			[[nodiscard]] std::unique_ptr<ConnectionBody> DeepCopy() const override
			{
				auto conn = std::make_unique<ConnectionBodyOverride>();
				conn->mWeakSignal = mWeakSignal;
				conn->mSlotIndex = mSlotIndex;
				return conn;
			}

		private:
			WeakSignal mWeakSignal;
			std::optional<U32> mSlotIndex;
		};
	}

	class Connection
	{
		using ConnectionType = SingalImpl::ConnectionBody;	
	public:
		Connection() = default;
		~Connection();

		explicit Connection(std::unique_ptr<ConnectionType> && body);

		Connection(const Connection & connection);
		Connection(Connection && connection) = default;
		Connection& operator=(const Connection & connection);
		Connection& operator=(Connection && connection) = default;

		void Disconnect();
		bool IsConnected() const;

	private:
		std::unique_ptr<ConnectionType> mConnection = nullptr;

	};

	class ScopedConnection
	{
	public:
		ScopedConnection() = default;
		ScopedConnection(const ScopedConnection&) = delete;
		ScopedConnection(ScopedConnection&&) = default;
		ScopedConnection(const Connection & connection);
		ScopedConnection(Connection && connection);

		~ScopedConnection();

		ScopedConnection& operator=(const ScopedConnection&) = delete;
		ScopedConnection& operator=(ScopedConnection&&) = default;
		ScopedConnection& operator=(const Connection & rhs);
		ScopedConnection& operator=(Connection && rhs);

		void Disconnect();
		bool IsConnected() const;

	private:
		Connection mConnection;
	};
}
