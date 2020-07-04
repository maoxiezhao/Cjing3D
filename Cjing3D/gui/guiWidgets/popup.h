#pragma once

#include "gui\guiWidgets\container.h"
#include "gui\guiWidgets\button.h"

namespace Cjing3D {
	namespace Gui {
		class Popup : public Container
		{
		public:
			enum Side
			{
				Down = 0,
				Left,
				Right,
				Up
			};

			Popup(GUIStage& stage, const StringID& name = StringID::EMPTY, const F32x2& fixedSize = {0.0f, 0.0f});

			void ShowPopup(WidgetPtr attachWidget);
			void HidePopup();

			void SetAnchorPos(const F32x2 anchorPos);
			F32x2 GetAnchorPos()const { return mAnchorPos; }
			void SetSide(Side side) { mPopupSide = side; }
			Side GetSide()const { return mPopupSide; }
			void SetAttachWidget(WidgetPtr widget) { mAttachWidget = widget; }
			WidgetPtr GetAttachWidget()const { return mAttachWidget.lock(); }

		private:
			void RefreshPlacement();

			virtual void UpdateLayoutImpl(const Rect& destRect);
			virtual void FixedUpdateImpl();
			virtual void RenderImpl(const Rect& destRect);

			std::weak_ptr<Widget> mAttachWidget;
			ScopedConnection mConnection;

			F32x2 mAnchorPos;
			Side mPopupSide = Down;
			bool mIsPosDirty = true;
			Sprite mBgSprite;
		};

		class PopupButton : public Button
		{
		public:
			PopupButton(GUIStage& stage, const StringID& name = StringID::EMPTY, const UTF8String& text = "");
		
			std::shared_ptr<Popup> GetPopup() { return mPopup; }
			const std::shared_ptr<Popup> GetPopup()const { return mPopup; }

			void UpdatePopupAnchor();

		private:
			virtual void OnMouseClick(const VariantArray& variants);
			virtual void RenderImpl(const Rect& destRect);

			std::shared_ptr<Popup> mPopup = nullptr;
		};
	}
}