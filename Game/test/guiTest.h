#pragma once

#include "definitions\definitions.h"
#include "core\gameComponent.h"
#include "utils\signal\connectionList.h"

namespace Cjing3D
{
	class GUITest
	{
	public:
		GUITest() = default;
		void DoTest();

	private:
		ConnectionList mConnections;
	};
}