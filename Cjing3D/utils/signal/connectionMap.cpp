#include "connectionMap.h"

namespace Cjing3D
{
	ConnectionMap::~ConnectionMap()
	{
		Disconnect();
	}


	void ConnectionMap::Disconnect(const StringID& name)
	{
		auto it = mConnections.find(name);
		if (it != mConnections.end())
		{
			it->second.Disconnect();
			mConnections.erase(it);
		}
	}

	void ConnectionMap::Disconnect()
	{
		for (auto& kvp : mConnections) {
			kvp.second.Disconnect();
		}
		mConnections.clear();
	}
}