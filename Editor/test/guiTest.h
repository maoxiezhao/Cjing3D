#pragma once

#include "definitions\definitions.h"
#include "utils\signal\connectionList.h"
#include "gui\guiAnimation\guiTimeline.h"

namespace Cjing3D
{
	class GUITest
	{
	public:
		GUITest() = default;
		void DoTest();

	private:
		Gui::TimeLine mTimeLine;
		ConnectionList mConnections;
	};
}