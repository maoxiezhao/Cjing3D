#include "guiStage.h"
#include "guiRenderer.h"
#include "imguiStage.h"

namespace Cjing3D
{
	namespace {

#ifdef CJING_IMGUI_ENABLE
		IMGUIStage mImGuiStage;
#endif
	}

	GUIStage::GUIStage(SystemContext & systemContext):
		SubSystem(systemContext),
		mRenderer()
	{
	}

	GUIStage::~GUIStage()
	{
	}

	void GUIStage::Initialize()
	{
#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Initialize();
#endif

	}

	void GUIStage::Uninitialize()
	{
#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Uninitialize();
#endif

	}

	void GUIStage::Render()
	{
#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Render(mRenderer);
#endif

	}

	void GUIStage::Update(F32 deltaTime)
	{
#ifdef CJING_IMGUI_ENABLE
		mImGuiStage.Update(deltaTime);
#endif

	}
}