#pragma once

#include "common\common.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx11.h"

namespace Cjing3D
{
	class IMGUIStage;
	namespace Editor
	{
		void InitializeEditor(IMGUIStage& imguiStage);
		void ShowBasicWindow(F32 deltaTime);
	}
}