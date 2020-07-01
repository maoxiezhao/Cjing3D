#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class ScrollBar : public Widget
	{
	public:
		enum ScrollBarState
		{
			ScrollBarState_Base = 1 << 1,
			ScrollBarState_Hovered = 1 << 2,
			ScrollBarState_Pressed = 1 << 3
		};

		ScrollBar(GUIStage& stage, const StringID& name = StringID::EMPTY);

		void SetScrollValue(F32 value, bool ignoreParentEvent = false);
		F32 GetScrollValue() { return mScrollValue; }
		void SetScrollSpeed(F32 speed) { mScrollSpeed = speed; }
		F32 GetScrollSpeed()const { return mScrollSpeed; }
		void SetMaximum(F32 maximum) { mScrollMaximum = maximum; }
		F32 GetMaximum()const { return mScrollMaximum; }
		void SetMinimum(F32 minimum) { mScrollMinimum = minimum; }
		F32 GetMinimum()const { return mScrollMinimum; }
		void SetScrollPageHeight(F32 height) { mScrollPageHeight = height; }
		F32 GetScrollPageHeight()const { return mScrollPageHeight; }

	protected:
		Rect CalculateThumbRect(F32x2 pos);

		virtual void RenderImpl(const Rect& destRect);
		virtual void OnMouseEnter(const VariantArray& variants);
		virtual void OnMouseLeave(const VariantArray& variants);
		virtual void OnMousePressed(const VariantArray& variants);
		virtual void OnMouseReleased(const VariantArray& variants);
		virtual void OnMouseDrag(bool& handle, const VariantArray& variants);

	private:
		U32 mState = ScrollBarState::ScrollBarState_Base;
		F32 mScrollValue = 0.0f;
		F32 mScrollMinimum = 0.0f;
		F32 mScrollMaximum = 100.0f;
		F32 mScrollSpeed = 1.0f;
		F32 mScrollPageHeight = 1.0f;
		F32 mScrollSliderMinHeight = 10.0f;

		bool mIsDragging = false;
		F32 mDraggingPosY = 0.0f;

		Sprite mBarSprite;
		Sprite mThumbSprite;
	};

	class ScrollView : public Widget
	{
	public:
		ScrollView(GUIStage& stage, const StringID& name = StringID::EMPTY, F32x2 size = F32x2(0.0f, 0.0f), const WidgetMargin& margin = WidgetMargin());

		void SetWidget(WidgetPtr widget);
		WidgetPtr GetWidget();

		void SetScrollSpeed(F32 speed);
		F32 GetScrollSpeed()const;
		void SetScrollValue(F32 value);
		F32 GetScrollValue()const;
		void ScrollToTop();
		void ScrollToEnd();
		void SetMargin(const WidgetMargin& margin) { mMargin = margin; }

		virtual void SetNeedLayout(bool needLayout);
		virtual F32x2 CalculateBestSize()const;
		virtual void UpdateLayout();
		virtual void Render(const F32x2& offset);

	protected:
		virtual void OnMouseScroll(const VariantArray& variants);
		virtual void Add(WidgetPtr node);
		virtual void Remove(WidgetPtr node);

	private:
		void ConnectWheelScrollSignal(bool isConnected);
		bool mIsWheelScrollConnected = false;

		bool mNeedLayout = true;
		WidgetMargin mMargin;
		WidgetPtr mWidget = nullptr;
		std::shared_ptr<ScrollBar> mScrollBar = nullptr;
	};

}
}