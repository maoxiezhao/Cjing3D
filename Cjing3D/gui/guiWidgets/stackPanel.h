#pragma once

#include "gui\guiWidgets\container.h"

namespace Cjing3D {
namespace Gui {

	class StackPanel : public Container
	{
	public:
		StackPanel(GUIStage& stage, const StringID& name = StringID::EMPTY, F32 width = 1.0f, F32 height = 1.0f);
		virtual ~StackPanel();

		virtual void RenderImpl(const Rect& destRect);

	private:
		Sprite mBgSprite;
	};

}
}