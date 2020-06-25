#include "listPanel.h"
#include "widgetInclude.h"

namespace Cjing3D {
namespace Gui {

	ListItem::ListItem(GUIStage& stage) :
		Widget(stage)
	{
		SetSize({ 74.0f, 22.0f });

		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER, std::bind(&ListItem::OnMouseEnter, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE, std::bind(&ListItem::OnMouseLeave, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, std::bind(&ListItem::OnMousePressed, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP, std::bind(&ListItem::OnMouseReleased, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK, std::bind(&ListItem::OnMouseClick, this, std::placeholders::_4));
	}

	void ListItem::SetListPanel(const std::weak_ptr<Widget>& listPanel)
	{
		mListPanel = listPanel;
	}

	void ListItem::SetListItemIndex(U32 index)
	{
		mListItemIndex = index;
	}

	void ListItem::SetWidget(WidgetPtr widget)
	{
		if (mWidget != widget)
		{
			if (mWidget != nullptr) {
				Widget::Remove(mWidget);
			}

			mWidget = widget;

			if (widget != nullptr) {
				Widget::Add(widget);
			}
		}
	}

	WidgetPtr ListItem::GetWidget()
	{
		return mWidget;
	}

	void ListItem::SetSelected(bool isSelected)
	{
		if (isSelected)
		{
			mListItemState |= ListItemState_Selected;
		}
		else
		{
			mListItemState &= ~ListItemState_Selected;
		}
	}

	void ListItem::OnMouseEnter(const VariantArray& variants)
	{
		if (!IsEnabled()) {
			return;
		}
		mListItemState |= ListItemState_Hovered;
	}

	void ListItem::OnMouseLeave(const VariantArray& variants)
	{
		mListItemState &= ~ListItemState_Hovered;
	}

	void ListItem::OnMousePressed(const VariantArray& variants)
	{
		mListItemState |= ListItemState_Pressed;
	}

	void ListItem::OnMouseReleased(const VariantArray& variants)
	{
		mListItemState &= ~ListItemState_Pressed;
	}

	void ListItem::OnMouseClick(const VariantArray& variants)
	{
		if (!IsEnabled()) {
			return;
		}

		auto listPanelWidget = mListPanel.lock();
		if (listPanelWidget == nullptr) {
			return;
		}

		auto listPanel = std::dynamic_pointer_cast<ListPanel>(listPanelWidget);
		if (listPanel == nullptr) {
			return;
		}

		listPanel->SetCurrentIndex(mListItemIndex);
	}

	void ListItem::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		// render background 
		auto enabledState = (ListItemState_Selected | ListItemState_Hovered | ListItemState_Pressed);
		if (IsEnabled() && (mListItemState & enabledState))
		{
			Color4 bgColor = scheme.GetColor(GUIScheme::ListItemBackgroundHovered);
			if (mListItemState & ListItemState_Selected)
			{
				bgColor = scheme.GetColor(GUIScheme::ListItemBackgroundSelected);
			}
			else if (mListItemState & ListItemState_Pressed)
			{
				bgColor = scheme.GetColor(GUIScheme::ListItemBackgroundPressed);
			}

			mBgSprite.SetPos(destRect.GetPos());
			mBgSprite.SetSize(destRect.GetSize());
			mBgSprite.SetColor(bgColor);
			renderer.RenderSprite(mBgSprite);
		}
	}

	void ListItem::UpdateLayoutImpl(const Rect& destRect)
	{
	}

	ListPanel::ListPanel(GUIStage& stage, const StringID& name, F32 width, F32 height) :
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

	void ListPanel::Add(WidgetPtr node)
	{
		auto item = std::make_shared<ListItem>(GetGUIStage());
		item->SetWidget(node);
		item->SetListPanel(GetNodePtr());
		item->SetListItemIndex(mLayout->GetChildrenCount());

		mLayout->AddChild(node);
	}

	void ListPanel::Remove(WidgetPtr node)
	{
		if (node == nullptr) {
			return;
		}
		mLayout->RemoveChild(node);

		U32 currentIndex = 0;
		for (auto child : mLayout->GetChildren())
		{
			if (child != nullptr)
			{
				auto item = std::static_pointer_cast<ListItem>(child);
				item->SetListItemIndex(currentIndex++);
			}
		}
	}

	void ListPanel::SetCurrentIndex(U32 index)
	{
		mCurrentIndex = index;
	}

	U32 ListPanel::GetListItemCount() const
	{
		return mLayout->GetChildrenCount();
	}

	void ListPanel::RenderImpl(const Rect& destRect)
	{
	}
}
}