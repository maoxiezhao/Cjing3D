#include "scrollView.h"
#include "widgetInclude.h"

namespace Cjing3D {
namespace Gui {

	ScrollBar::ScrollBar(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
	}

	void ScrollBar::SetValue(F32 value)
	{
	}

	void ScrollBar::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		mBarSprite.SetPos(destRect.GetPos());
		mBarSprite.SetSize(destRect.GetSize());
		mBarSprite.SetColor(scheme.GetColor(GUIScheme::ScrollBarBase));

		renderer.RenderSprite(mBarSprite);
	}

	void ScrollBar::OnMouseEnter(const VariantArray& variants)
	{
	}

	void ScrollBar::OnMouseLeave(const VariantArray& variants)
	{
	}

	void ScrollBar::OnMousePressed(const VariantArray& variants)
	{
	}

	void ScrollBar::OnMouseReleased(const VariantArray& variants)
	{
	}

	ScrollView::ScrollView(GUIStage& stage, const StringID& name, F32x2 size, const WidgetMargin& margin) :
		Widget(stage, name),
		mMargin(margin)
	{
		SetSizeAndFixedSize(size);

		// init scroll bar
		mScrollBar = std::make_shared<ScrollBar>(stage, "scrollBar");
		mScrollBar->SetSizeAndFixedSize({10.0f, 20.0f});
		mScrollBar->SetVisible(true);
		mScrollBar->SetValue(0.0f);
		Widget::Add(mScrollBar);
	}

	void ScrollView::SetWidget(WidgetPtr widget)
	{
		if (widget->GetParent() != nullptr) 
		{
			Debug::Warning("[ScrollView::SetWidget] invalid widget with parent.");
			return;
		}


		mWidget = widget;
		Widget::Add(widget);
	}

	WidgetPtr ScrollView::GetWidget()
	{
		return mWidget;
	}

	void ScrollView::SetScrollValue(F32 value)
	{
		if (!IsF32EqualF32(mScrollValue, value))
		{
			mScrollValue = value;
			mNeedLayout = true;
		}
	}

	void ScrollView::ScrollToTop()
	{
		if (mWidget != nullptr)
		{
			F32x2 childSize = mWidget->GetBestSize();
			if (childSize > GetSize()) {
				SetScrollValue(mScrollMinimum);
			}
		}
	}

	void ScrollView::ScrollToEnd()
	{
		if (mWidget != nullptr)
		{
			F32x2 childSize = mWidget->GetBestSize();
			if (childSize > GetSize()) {
				SetScrollValue(mScrollMaximum);
			}
		}
	}

	void ScrollView::OnMouseScroll(const VariantArray& variants)
	{
		I32 delta = variants[2].GetValue<I32>();
		const F32 newValue = -delta * mScrollSpeed / 400.0f + mScrollValue;
		SetScrollValue(std::clamp(newValue, mScrollMinimum, mScrollMaximum));
	}

	void ScrollView::Add(WidgetPtr node)
	{
		// do nothing
	}

	void ScrollView::Remove(WidgetPtr node)
	{
		// do nothing
	}

	void ScrollView::ConnectWheelScrollSignal(bool isConnected)
	{
		if (isConnected != mIsWheelScrollConnected)
		{
			mIsWheelScrollConnected = isConnected;
			if (isConnected)
			{
				ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_SCROLL, std::bind(&ScrollView::OnMouseScroll, this, std::placeholders::_4), Dispatcher::back_post_child);
			}
			else
			{
				DisconnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_SCROLL, std::bind(&ScrollView::OnMouseScroll, this, std::placeholders::_4), Dispatcher::back_post_child);
			}
		}
	}

	F32x2 ScrollView::CalculateBestSize() const
	{
		return F32x2();
	}

	void ScrollView::UpdateLayout()
	{
		if (!IsVisible()) {
			return;
		}

		if (mWidget == nullptr)
		{
			mScrollBar->SetVisible(false);
			return;
		}

		// child layout
		F32x2 marginSize = {mMargin.left + mMargin.right, mMargin.top + mMargin.bottom};
		F32x2 childSize = mWidget->GetBestSize();
		F32x2 parentSize = GetSize();
		if (childSize[1] > parentSize[1])
		{
			mWidget->SetSize({
				parentSize[0] - mScrollBar->GetWidth() - marginSize[0],
				childSize[1]
			});
			mWidget->SetPos({ mMargin.left, mMargin.top - mScrollValue });

			F32 oldMaximum = mScrollMinimum;
			mScrollSpeed = parentSize[1] * 0.5f;
			mScrollMinimum = 0.0f;
			mScrollMaximum = childSize[1] - parentSize[1];

			if (oldMaximum > mScrollMaximum) {
				mScrollValue = std::min(mScrollValue, mScrollMaximum);
			}

			mScrollBar->SetPos({ parentSize[1] - mScrollBar->GetSize()[1], 0.0f });
			mScrollBar->SetVisible(true);

			ConnectWheelScrollSignal(true);
		}
		else
		{
			mWidget->SetSize(GetSize());
			mWidget->SetPos({ mMargin.left, mMargin.top});
			mScrollBar->SetVisible(false);

			ConnectWheelScrollSignal(false);
		}

		mWidget->UpdateLayout();
		mNeedLayout = false;
	}

	void ScrollView::Render(const F32x2& offset)
	{
		if (!IsVisible()) {
			return;
		}

		if (mNeedLayout) {
			UpdateLayout();
		}

		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		Rect destRect(GetArea());
		destRect.Offset(offset);

		if (GetGUIStage().IsDebugDraw())
		{
			if (mDebugSprite.GetColor() == Color4::White()) {
				Color4 randomColor;
				randomColor.SetA(255);
				randomColor.SetR(Random::GetRandomInt(255));
				randomColor.SetG(Random::GetRandomInt(255));
				randomColor.SetB(Random::GetRandomInt(255));
				mDebugSprite.SetColor(randomColor);
			}

			mDebugSprite.SetPos(destRect.GetPos());
			mDebugSprite.SetSize(destRect.GetSize());
			renderer.RenderSprite(mDebugSprite);
		}

		// render impl
		mDebugSprite.SetPos(destRect.GetPos());
		mDebugSprite.SetSize(destRect.GetSize());
		mDebugSprite.SetColor(Color4::White());
		renderer.RenderSprite(mDebugSprite);

		// render children
		renderer.ApplyScissor(destRect);
		F32x2 childOffset = destRect.GetPos();
		if (mWidget != nullptr) {
			mWidget->Render(childOffset);
		}

		if (mScrollBar->IsVisible()) {
			mScrollBar->Render(childOffset);
		}

		renderer.ResetScissor();
	}
}
}