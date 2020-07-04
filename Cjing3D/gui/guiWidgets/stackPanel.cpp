#include "stackPanel.h"
#include "widgetInclude.h"
#include "gui\guiWidgets\layouts\boxLayout.h"

namespace Cjing3D{
namespace Gui {

	StackPanel::StackPanel(GUIStage& stage, const StringID& name, F32 width, F32 height) :
		Container(stage, name)
	{
		SetSize(F32x2(width, height));
		SetLayout(std::make_shared<BoxLayout>(AlignmentOrien::AlignmentOrien_Vertical, AlignmentMode::AlignmentMode_Center));
	}

	StackPanel::~StackPanel()
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