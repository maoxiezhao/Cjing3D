#pragma once

#include "gui\guiWidgets\widgets.h"
#include "gui\guiWidgets\verticalLayout.h"

namespace Cjing3D {
namespace Gui {

	class StackPanel : public Widget
	{
	public:
		StackPanel(GUIStage& stage, const StringID& name = StringID::EMPTY, F32 width = 1.0f, F32 height = 1.0f);
		virtual ~StackPanel();

		virtual void Add(WidgetPtr node);
		virtual void Remove(WidgetPtr node);

	protected:
		virtual void RenderImpl(const Rect& destRect);

	private:
		Sprite mBgSprite;
		std::shared_ptr<VerticalLayout> mLayout = nullptr;
	};

}
}