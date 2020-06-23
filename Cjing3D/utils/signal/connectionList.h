// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.
#pragma once

#include "utils\signal\signalInclude.h"
#include "utils\signal\connection.h"
#include "utils\signal\signal.h"

namespace Cjing3D
{
	class ConnectionList final
	{
	public:
		ConnectionList() = default;
		ConnectionList(ConnectionList&&) = default;
		ConnectionList& operator=(ConnectionList&&) = default;
		~ConnectionList();

		ConnectionList(const ConnectionList&) = delete;
		ConnectionList& operator=(const ConnectionList&) = delete;

		void operator+=(Connection&& connection);

		template <typename... Args, typename Func>
		Connection operator()(Signal<void(Args...)>& signal, Func&& func)
		{
			auto connection = signal.Connect(std::forward<Func>(func));
			mConnections.push_back(connection);
			return connection;
		}

		void Disconnect();

	private:
		std::vector<Connection> mConnections;

	};
}