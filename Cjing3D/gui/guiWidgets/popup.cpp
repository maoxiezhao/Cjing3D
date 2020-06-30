#include "popup.h"
#include "gui\guiWidgets\widgetInclude.h"

namespace Cjing3D
{
namespace Gui
{
	Popup::Popup(GUIStage& stage, const StringID& name, const F32x2& fixedSize) :
		Widget(stage, name)
	{
		SetSize(fixedSize);
		SetFixedSize(fixedSize);
	}

	void Popup::ShowPopup(WidgetPtr attachWidget)
	{
		if (attachWidget == nullptr) {
			return;
		}

		SetAttachWidget(attachWidget);

		mConnection = GetWidgetHierarchy().ConnectFocusedWidgetChanged(
			[this](WidgetPtr oldWidget, WidgetPtr newWidge) {

				// 当新的focused Widget不等于按钮或者popup的时候，则关闭popup
				const auto CheckFocused = [&]()->bool {
					if (newWidge == mAttachWidget.lock()) {
						return true;
					}
					if (newWidge == nullptr) {
						return false;
					}

					auto curWidget = newWidge.get();
					while (curWidget != nullptr)
					{
						if (curWidget == this) {
							return true;
						}
						curWidget = curWidget->GetParent();
					}
					return false;
				};

				if (!CheckFocused()) {
					HidePopup();
					mConnection.Disconnect();
				}
			});

		GetWidgetHierarchy().AddWidget(GetNodePtr());
		GetWidgetHierarchy().SetCurrentFocusedWidget(GetNodePtr());

		SetVisible(true);
		SetSize(GetBestSize());
	}

	void Popup::HidePopup()
	{
		GetWidgetHierarchy().RemoveWidget(this->GetNodePtr());
		SetVisible(false);
	}

	void Popup::SetAnchorPos(const F32x2 anchorPos)
	{
		mAnchorPos = anchorPos;
		mIsPosDirty = true;
	}

	void Popup::UpdateLayoutImpl(const Rect& destRect)
	{
		switch (mPopupSide)
		{
		case Cjing3D::Gui::Popup::Down:
			break;
		case Cjing3D::Gui::Popup::Left:
			mAnchorPos[0] -= GetSize()[0];
			break;
		case Cjing3D::Gui::Popup::Right:
			break;
		case Cjing3D::Gui::Popup::Up:
			mAnchorPos[1] -= GetSize()[1];
			break;
		default:
			break;
		}
	}

	void Popup::RefreshPlacement()
	{
		if (mAttachWidget.lock() != nullptr) {
			SetPos(mAttachWidget.lock()->GetGlobalPos() + mAnchorPos);
		}
	}

	void Popup::FixedUpdateImpl()
	{
		RefreshPlacement();
	}

	void Popup::RenderImpl(const Rect& destRect)
	{
		GUIRenderer& renderer = GetGUIRenderer();
		const GUIScheme& scheme = renderer.GetGUIScheme();

		mBgSprite.SetPos(destRect.GetPos());
		mBgSprite.SetSize(destRect.GetSize());
		mBgSprite.SetColor(scheme.GetColor(GUIScheme::PopupPanelBackground));

		renderer.RenderSprite(mBgSprite);
	}

	PopupButton::PopupButton(GUIStage& stage, const StringID& name, const UTF8String& text) :
		Button(stage, name, text)
	{
		mPopup = std::make_shared<Popup>(stage, STRING_ID(Popup));
		mPopup->SetSize(F32x2(320.0f, 250.0f));
		mPopup->SetHierarchySortOrder(HierarchySortOrder::Front);
		mPopup->SetVisible(false);
	}

	void PopupButton::UpdatePopupAnchor()
	{
		F32x2 buttonSize = GetSize();
		F32x2 popupSize = mPopup->GetSize();
		Popup::Side side = mPopup->GetSide();
		switch (side)
		{
		case Cjing3D::Gui::Popup::Down:
			mPopup->SetAnchorPos({ (buttonSize[0] - popupSize[0]) * 0.5f, buttonSize[1] + 0.5f });
			break;
		case Cjing3D::Gui::Popup::Left:
			mPopup->SetAnchorPos({ 0.0f - 5.0f, (buttonSize[1] - popupSize[1]) * 0.5f });
			break;
		case Cjing3D::Gui::Popup::Right:
			mPopup->SetAnchorPos({ buttonSize[0], (buttonSize[1] - popupSize[1]) * 0.5f });
			break;
		case Cjing3D::Gui::Popup::Up:
			mPopup->SetAnchorPos({ (buttonSize[0] - popupSize[0]) * 0.5f, 0.0f - 0.5f });
			break;
		default:
			break;
		}
	}

	void PopupButton::OnMouseClick(const VariantArray& variants)
	{
		if (!mPopup->IsVisible()) 
		{
			mPopup->ShowPopup(GetNodePtr());
			UpdatePopupAnchor();
			mPopup->UpdateLayout();
		}
		else
		{
			mPopup->HidePopup();
		}
	}

	void PopupButton::RenderImpl(const Rect& destRect)
	{
		Button::RenderImpl(destRect);
	}
}
}