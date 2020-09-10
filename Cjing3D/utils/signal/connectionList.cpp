// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.
#include "connectionList.h"

namespace Cjing3D
{
	ConnectionList::~ConnectionList()
	{
		Disconnect();
	}

	void ConnectionList::operator+=(Connection&& connection)
	{
		mConnections.push_back(std::move(connection));
	}

	void ConnectionList::Disconnect()
	{
		for (auto& connection : mConnections) {
			connection.Disconnect();
		}
		mConnections.clear();
	}
}