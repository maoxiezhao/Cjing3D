#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class StackPanel : public Widget
	{
	public:
		StackPanel(GUIStage& stage, const StringID& name = StringID::EMPTY, F32 width = 1.0f, F32 height = 1.0f);
		virtual ~StackPanel();

		virtual void OnLoaded();
		virtual void OnUnloaded();
		virtual void Update(F32 dt);
		virtual void FixedUpdate();

	private:
		virtual void RenderImpl(const Rect& destRect);

	private:
		Sprite mBgSprite;
	};

}
}