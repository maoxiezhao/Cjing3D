#include "stackPanel.h"
#include "widgetInclude.h"

namespace Cjing3D{
namespace Gui {

	StackPanel::StackPanel(GUIStage& stage, const StringID& name, F32 width, F32 height) :
		Widget(stage, name)
	{
		SetSize(F32x2(width, height));
		SetIsAlwaysLayout(true);

		mLayout = std::make_shared<VerticalLayout>(stage, "verticalLayout", width, height);
		mLayout->SetLayoutSpacing(10.0f);
		mLayout->SetMargin(2.0f, 2.0f, 2.0f, 2.0f);
		mLayout->SetVisible(true);

		Widget::Add(mLayout);
	}

	StackPanel::~StackPanel()
	{
	}

	void StackPanel::Add(WidgetPtr node)
	{
		mLayout->AddChild(node);
	}

	void StackPanel::Remove(WidgetPtr node)
	{
		mLayout->RemoveChild(node);
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