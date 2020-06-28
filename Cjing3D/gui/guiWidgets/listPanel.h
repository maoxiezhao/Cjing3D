#pragma once

#include "gui\guiWidgets\widgets.h"

#include <optional>

namespace Cjing3D {
namespace Gui {
	
	class ListItem : public Widget
	{
	public:
		enum ListItemState
		{
			ListItemState_Base = 1 << 1,
			ListItemState_Hovered = 1 << 2,
			ListItemState_Pressed = 1 << 3,
			ListItemState_Selected = 1 << 4
		};
		ListItem(GUIStage& stage);

		void SetListPanel(const std::weak_ptr<Widget>& listPanel);
		void SetListItemIndex(U32 index);
		void SetWidget(WidgetPtr widget);
		WidgetPtr GetWidget();
		void SetSelected(bool isSelected);

	private:
		void OnMouseEnter(const VariantArray& variants);
		void OnMouseLeave(const VariantArray& variants);
		void OnMousePressed(const VariantArray& variants);
		void OnMouseReleased(const VariantArray& variants);
		void OnMouseClick(const VariantArray& variants);

		virtual void RenderImpl(const Rect& destRect);

	private:
		U32 mListItemIndex = 0;
		U32 mListItemState = ListItemState_Base;
		Sprite mBgSprite;
		std::weak_ptr<Widget> mListPanel;
		WidgetPtr mWidget = nullptr;
	};

	class ListPanel : public Widget
	{
	public:
		ListPanel(GUIStage& stage, const StringID& name = StringID::EMPTY, F32 width = 1.0f, F32 height = 1.0f);

		virtual void Add(WidgetPtr node);
		virtual void Remove(WidgetPtr node);

		void SetCurrentIndex(U32 index);
		U32 GetListItemCount()const;
		std::optional<U32> GetCurrentIndex()const;
		WidgetPtr GetChildAt(U32 index);
		void SetMargin(F32 left, F32 top, F32 right, F32 bottom);
		void SetLayoutSpacing(F32 spacing);

		Signal<void(std::optional<U32> index)> OnCurrentIndexChangedCallback;

	protected:
		virtual void RenderImpl(const Rect& destRect);

	private:
		Sprite mBgSprite;
		WidgetMargin mMargin;
		std::vector<std::shared_ptr<ListItem>> mListItems;
		std::optional<U32> mCurrentIndex;
	};
}
}