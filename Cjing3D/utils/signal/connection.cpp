// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.
#include "connection.h"
#include "signal.h"

namespace Cjing3D
{
	Connection::~Connection()
	{
	}

	Connection::Connection(std::unique_ptr<ConnectionType>&& body) :
		mConnection(std::move(body))
	{
	}

	Connection::Connection(const Connection& connection)
	{
		if (connection.mConnection != nullptr) {
			mConnection = connection.mConnection->DeepCopy();
		}
	}

	Connection& Connection::operator=(const Connection& connection)
	{
		if (connection.mConnection != nullptr) {
			mConnection = connection.mConnection->DeepCopy();
		}
		return *this;
	}

	void Connection::Disconnect()
	{
		if (mConnection != nullptr)
		{
			mConnection->Disconnect();
			mConnection.reset();
		}
	}

	bool Connection::IsConnected() const
	{
		return mConnection != nullptr && mConnection->Valid();
	}
}