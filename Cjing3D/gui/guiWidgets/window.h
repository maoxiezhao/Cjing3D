#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class Window : public Widget
	{
	public:
		Window(GUIStage& stage, const StringID& name = StringID::EMPTY, const UTF8String& title = "", F32x2 fixedSize = F32x2(0.0f, 0.0f));
		
		void SetTitle(const UTF8String& text);
		const UTF8String& GetTitle()const;
		void SetModal(bool isModal) { mIsModal = isModal; }
		bool IsModal()const { return mIsModal; }
		void SetCanDrag(bool canDrag) { mIsCanDrag = canDrag; }
		bool IsCanDrag()const { return mIsCanDrag; }
		bool IsDragging()const { return mIsDraging; }
		TextDrawable& GetTitleTextDrawable() { return mTitleText; }

		virtual F32x2 CalculateBestSize()const;

	protected:
		void OnMouseDrag(const VariantArray& variants);

		virtual void RenderImpl(const Rect& destRect);

	private:
		Sprite mBgSprite;
		Sprite mHeadSprite;
		TextDrawable mTitleText;
		bool mIsModal = false;
		bool mIsCanDrag = false;
		bool mIsDraging = false;
	};

}
}