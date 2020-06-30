#include "listPanel.h"
#include "widgetInclude.h"
#include "gui\guiWidgets\layouts\boxLayout.h"

namespace Cjing3D {
namespace Gui {

	ListItem::ListItem(GUIStage& stage) :
		Widget(stage)
	{
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_ENTER, std::bind(&ListItem::OnMouseEnter, this, std::placeholders::_4), Dispatcher::back_pre_child);
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_LEAVE, std::bind(&ListItem::OnMouseLeave, this, std::placeholders::_4), Dispatcher::back_post_child);
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_DOWN, std::bind(&ListItem::OnMousePressed, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_UP, std::bind(&ListItem::OnMouseReleased, this, std::placeholders::_4));
		ConnectSignal(UI_EVENT_TYPE::UI_EVENT_MOUSE_BUTTON_CLICK, std::bind(&ListItem::OnMouseClick, this, std::placeholders::_4));

		SetLayout(std::make_shared<BoxLayout>(AlignmentOrien::AlignmentOrien_Horizontal, AlignmentMode::AlignmentMode_Center));
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

	ListPanel::ListPanel(GUIStage& stage, const StringID& name, F32 width, F32 height) :
		Widget(stage, name)
	{
		SetSize(F32x2(width, height));
		SetLayout(std::make_shared<BoxLayout>(AlignmentOrien::AlignmentOrien_Vertical, AlignmentMode::AlignmentMode_Center));
	}

	void ListPanel::Add(WidgetPtr node)
	{
		auto item = std::make_shared<ListItem>(GetGUIStage());
		item->SetWidget(node);
		item->SetListPanel(GetNodePtr());
		item->SetListItemIndex(mListItems.size());
		item->SetVisible(true);

		Widget::Add(item);
		mListItems.push_back(item);
	}

	void ListPanel::Remove(WidgetPtr node)
	{
		if (node == nullptr) {
			return;
		}
		auto it = std::find_if(mListItems.begin(), mListItems.end(), [node](const std::shared_ptr<ListItem>& item) {
			return item->GetWidget() == node;
			});
		if (it == mListItems.end()) {
			return;
		}
		mListItems.erase(it);
		Widget::Remove(node);

		U32 currentIndex = 0;
		for (auto child : mListItems)
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
		if (*mCurrentIndex == index || index >= GetListItemCount()) {
			return;
		}

		if (mCurrentIndex != std::nullopt)
		{
			for (auto child : mListItems)
			{
				if (child != nullptr)
				{
					auto item = std::static_pointer_cast<ListItem>(child);
					item->SetSelected(false);
				}
			}
		}

		WidgetPtr widget = mListItems[index];
		if (widget == nullptr) {
			mCurrentIndex = std::nullopt;
			return;
		}

		auto item = std::static_pointer_cast<ListItem>(widget);
		item->SetSelected(true);

		mCurrentIndex = index;
		OnCurrentIndexChangedCallback(mCurrentIndex);
	}

	U32 ListPanel::GetListItemCount() const
	{
		return mListItems.size();
	}

	std::optional<U32> ListPanel::GetCurrentIndex() const
	{
		return mCurrentIndex;
	}

	WidgetPtr ListPanel::GetChildAt(U32 index)
	{
		auto widget = mListItems[index];
		if (widget == nullptr) {
			return nullptr;
		}

		auto item = std::static_pointer_cast<ListItem>(widget);
		if (item == nullptr) {
			return nullptr;
		}

		return item->GetWidget();
	}

	void ListPanel::SetMargin(F32 left, F32 top, F32 right, F32 bottom)
	{
		mMargin = { left , top, right, bottom };
		std::dynamic_pointer_cast<BoxLayout>(GetLayout())->SetMargin(mMargin);
	}

	void ListPanel::SetLayoutSpacing(F32 spacing)
	{
		std::dynamic_pointer_cast<BoxLayout>(GetLayout())->SetSpacing(spacing);
	}

	void ListPanel::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		mBgSprite.SetPos(destRect.GetPos());
		mBgSprite.SetSize(destRect.GetSize());
		mBgSprite.SetColor(scheme.GetColor(GUIScheme::ListPanelBackground));

		renderer.RenderSprite(mBgSprite);
	}
}
}