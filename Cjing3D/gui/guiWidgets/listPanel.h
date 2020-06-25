#pragma once

#include "gui\guiWidgets\widgets.h"
#include "gui\guiWidgets\verticalLayout.h"

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
		virtual void UpdateLayoutImpl(const Rect& destRect);

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

	protected:
		virtual void RenderImpl(const Rect& destRect);

	private:
		Sprite mBgSprite;
		std::shared_ptr<VerticalLayout> mLayout = nullptr;
		U32 mCurrentIndex = 0;
	};
}
}