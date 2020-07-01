#include "scrollView.h"
#include "widgetInclude.h"

namespace Cjing3D {
namespace Gui {

	namespace {
	
		F32 CalculateSliderHeight(F32 barHeight, F32 pageHeight, F32 minimum, F32 maximum)
		{
			return barHeight * (pageHeight / (maximum - minimum + pageHeight));
		}
	}

	ScrollBar::ScrollBar(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER, std::bind(&ScrollBar::OnMouseEnter, this, std::placeholders::_4), Dispatcher::back_child);
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE, std::bind(&ScrollBar::OnMouseLeave, this, std::placeholders::_4), Dispatcher::back_child);
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, std::bind(&ScrollBar::OnMousePressed, this, std::placeholders::_4), Dispatcher::back_child);
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP, std::bind(&ScrollBar::OnMouseReleased, this, std::placeholders::_4), Dispatcher::back_child);
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_DRAG, std::bind(&ScrollBar::OnMouseDrag, this, std::placeholders::_3, std::placeholders::_4), Dispatcher::back_child);
	}

	void ScrollBar::SetScrollValue(F32 value, bool ignoreParentEvent)
	{
		mScrollValue = value;

		if (!ignoreParentEvent && GetParent() != nullptr) {
			GetParent()->SetNeedLayout(true);
		}
	}

	void ScrollBar::OnMouseEnter(const VariantArray& variants)
	{
		mState |= ScrollBarState_Hovered;
	}

	void ScrollBar::OnMouseLeave(const VariantArray& variants)
	{
		mState &= ~ScrollBarState_Hovered;
	}

	void ScrollBar::OnMousePressed(const VariantArray& variants)
	{
		U32 buttonIndex = variants[2].GetValue<I32>();
		if (buttonIndex != 0) {
			return;
		}

		F32x2 pos = {
			(F32)variants[0].GetValue<I32>(),
			(F32)variants[1].GetValue<I32>()
		};
		F32x2 localPos = TransformToLocalCoord(pos);
		Rect thumbRect = CalculateThumbRect(F32x2());

		// 如果点击位置在thumbRect范围内，则开始拖拽行为
		if (thumbRect.Intersects(localPos))
		{
			mDraggingPosY = localPos.y();
			mIsDragging = true;
		}
		else
		{
			F32 sliderHeght = CalculateSliderHeight(GetHeight(), mScrollPageHeight, mScrollMinimum, mScrollMaximum);
			F32 ratio = (localPos.y() - sliderHeght * 0.5f) / (GetHeight() - sliderHeght);
			F32 newValue = std::clamp(ratio * (mScrollMaximum - mScrollMinimum), mScrollMinimum, mScrollMaximum);
			SetScrollValue(newValue);
		}
	}

	void ScrollBar::OnMouseReleased(const VariantArray& variants)
	{
		mIsDragging = false;
	}

	void ScrollBar::OnMouseDrag(bool& handle, const VariantArray& variants)
	{
		if (!mIsDragging) {
			return;
		}

		F32x2 pos = {
			(F32)variants[2].GetValue<I32>(),
			(F32)variants[3].GetValue<I32>()
		};
		F32x2 localPos = TransformToLocalCoord(pos);

		F32 sliderHeght = CalculateSliderHeight(GetHeight(), mScrollPageHeight, mScrollMinimum, mScrollMaximum);
		F32 ratio = (localPos.y() - mDraggingPosY) / (GetHeight() - sliderHeght);
		F32 newValue = std::clamp(ratio * (mScrollMaximum - mScrollMinimum), mScrollMinimum, mScrollMaximum);
		SetScrollValue(newValue);

		handle = true;
	}

	Rect ScrollBar::CalculateThumbRect(F32x2 pos)
	{
		F32 sliderHeight = CalculateSliderHeight(GetHeight(), mScrollPageHeight, mScrollMinimum, mScrollMaximum);
		sliderHeight = std::max(sliderHeight, mScrollSliderMinHeight);
		F32 offsetY = (mScrollValue - mScrollMinimum) / (mScrollMaximum - mScrollMinimum) * (GetHeight() - sliderHeight);

		F32 left = pos.x();
		F32 top = pos.y() + offsetY;
		return Rect(
			left,
			top,
			left + GetWidth(),
			top + sliderHeight
		);
	}

	void ScrollBar::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		// scroll bar
		mBarSprite.SetPos(destRect.GetPos());
		mBarSprite.SetSize(destRect.GetSize());
		mBarSprite.SetColor(scheme.GetColor(GUIScheme::ScrollBarBase));
		renderer.RenderSprite(mBarSprite);

		// scroll thumb
		Color4 sliderColor = scheme.GetColor(GUIScheme::ScrollSliderBase);
		if (mState & ScrollBarState_Hovered) {
			sliderColor = scheme.GetColor(GUIScheme::ScrollSliderHovered);
		}

		Rect thumbRect = CalculateThumbRect(destRect.GetPos());
		mThumbSprite.SetPos(thumbRect.GetPos());
		mThumbSprite.SetSize(thumbRect.GetSize());
		mThumbSprite.SetColor(sliderColor);
		renderer.RenderSprite(mThumbSprite);
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
		mScrollBar->SetScrollValue(0.0f, true);
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

	void ScrollView::SetScrollSpeed(F32 speed)
	{
		mScrollBar->SetScrollSpeed(speed);
	}

	F32 ScrollView::GetScrollSpeed() const
	{
		return mScrollBar->GetScrollSpeed();
	}

	void ScrollView::SetScrollValue(F32 value)
	{
		if (!IsF32EqualF32(mScrollBar->GetScrollValue(), value))
		{
			mScrollBar->SetScrollValue(value, true);
			mNeedLayout = true;
		}
	}

	F32 ScrollView::GetScrollValue() const
	{
		return mScrollBar->GetScrollValue();
	}

	void ScrollView::ScrollToTop()
	{
		if (mWidget != nullptr)
		{
			F32x2 childSize = mWidget->GetBestSize();
			if (childSize > GetSize()) {
				SetScrollValue(mScrollBar->GetMinimum());
			}
		}
	}

	void ScrollView::ScrollToEnd()
	{
		if (mWidget != nullptr)
		{
			F32x2 childSize = mWidget->GetBestSize();
			if (childSize > GetSize()) {
				SetScrollValue(mScrollBar->GetMaximum());
			}
		}
	}

	void ScrollView::OnMouseScroll(const VariantArray& variants)
	{
		I32 delta = variants[2].GetValue<I32>();
		const F32 newValue = -delta * mScrollBar->GetScrollSpeed() * mScrollBar->GetScrollPageHeight() / 400.0f + mScrollBar->GetScrollValue();
		SetScrollValue(std::clamp(
			newValue, 
			mScrollBar->GetMinimum(), 
			mScrollBar->GetMaximum()));
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
			if (isConnected) {
				ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_SCROLL, std::bind(&ScrollView::OnMouseScroll, this, std::placeholders::_4), Dispatcher::back_post_child);
			}
			else {
				DisconnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_SCROLL, std::bind(&ScrollView::OnMouseScroll, this, std::placeholders::_4), Dispatcher::back_post_child);
			}
		}
	}

	void ScrollView::SetNeedLayout(bool needLayout)
	{
		mNeedLayout = needLayout;
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
			auto scrollValue = mScrollBar->GetScrollValue();
			mWidget->SetSize({
				parentSize[0] - mScrollBar->GetWidth() - marginSize[0],
				childSize[1]
			});
			mWidget->SetPos({ mMargin.left, mMargin.top - scrollValue });

			F32 oldMaximum = mScrollBar->GetMaximum();
			mScrollBar->SetScrollPageHeight(parentSize[1]);
			mScrollBar->SetMinimum(0.0f);
			mScrollBar->SetMaximum(childSize[1] - parentSize[1]);

			if (oldMaximum > mScrollBar->GetMaximum()) {
				mScrollBar->SetScrollValue(std::min(scrollValue, mScrollBar->GetMaximum()));
			}

			mScrollBar->SetSize({ mScrollBar->GetWidth(), GetHeight() });
			mScrollBar->SetPos({ parentSize[0] - mScrollBar->GetSize()[0], 0.0f });
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