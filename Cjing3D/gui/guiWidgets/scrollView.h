#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class ScrollBar : public Widget
	{
	public:
		ScrollBar(GUIStage& stage, const StringID& name = StringID::EMPTY);

		void SetValue(F32 value);
		F32 GetValue() { return mScrollValue; }

	protected:
		virtual void RenderImpl(const Rect& destRect);
		virtual void OnMouseEnter(const VariantArray& variants);
		virtual void OnMouseLeave(const VariantArray& variants);
		virtual void OnMousePressed(const VariantArray& variants);
		virtual void OnMouseReleased(const VariantArray& variants);

	private:
		F32 mScrollValue = 0.0f;
		Sprite mBarSprite;
	};

	class ScrollView : public Widget
	{
	public:
		ScrollView(GUIStage& stage, const StringID& name = StringID::EMPTY, F32x2 size = F32x2(0.0f, 0.0f), const WidgetMargin& margin = WidgetMargin());

		void SetWidget(WidgetPtr widget);
		WidgetPtr GetWidget();

		void SetScrollSpeed(F32 speed) { mScrollSpeed = speed; }
		F32 GetScrollSpeed()const { return mScrollSpeed; }
		void SetScrollValue(F32 value);
		F32 GetScrollValue()const { return mScrollValue; }
		void ScrollToTop();
		void ScrollToEnd();
		void SetMargin(const WidgetMargin& margin) { mMargin = margin; }

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
		F32 mScrollMinimum = 0.0f;
		F32 mScrollMaximum = 100.0f;
		F32 mScrollValue = 0.0f;
		F32 mScrollSpeed = 1.0f;
		WidgetMargin mMargin;

		WidgetPtr mWidget = nullptr;
		std::shared_ptr<ScrollBar> mScrollBar = nullptr;

		Sprite mDebugSprite;
	};

}
}