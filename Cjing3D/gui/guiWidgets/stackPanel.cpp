#include "stackPanel.h"
#include "widgetInclude.h"

namespace Cjing3D{
namespace Gui {

	StackPanel::StackPanel(GUIStage& stage, const StringID& name, F32 width, F32 height) :
		Widget(stage, name)
	{
		SetSize(F32x2(width, height));
	}

	StackPanel::~StackPanel()
	{
	}

	void StackPanel::OnLoaded()
	{
	}

	void StackPanel::OnUnloaded()
	{
	}

	void StackPanel::Update(F32 dt)
	{
	}

	void StackPanel::FixedUpdate()
	{
	}

	void StackPanel::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		mBgSprite.SetPos(destRect.GetPos());
		mBgSprite.SetSize(destRect.GetSize());
		mBgSprite.SetColor(scheme.GetColor(GUIScheme::PanelBackground));

		renderer.RenderSprite(mBgSprite);
	}

}
}