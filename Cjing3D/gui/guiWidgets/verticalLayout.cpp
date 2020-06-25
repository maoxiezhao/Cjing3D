#include "verticalLayout.h"
#include "widgetInclude.h"

namespace Cjing3D {
namespace Gui {

	VerticalLayout::VerticalLayout(GUIStage& stage, const StringID& name, F32 width, F32 height):
		Widget(stage)
	{
		SetSize(F32x2(width, height));
	}

	bool VerticalLayout::IsNeedLayout() const
	{
		if (mIsNeedLayout ) {
			return true;
		}

		for (auto& child : mChildren)
		{
			if (child->IsNeedLayout()) {
				return true;
			}
		}
		return false;
	}

	void VerticalLayout::SetMargin(F32 left, F32 top, F32 right, F32 bottom)
	{
		mMargin.left = left;
		mMargin.top = top;
		mMargin.right = right;
		mMargin.bottom = bottom;
	}

	void VerticalLayout::SetLayoutSpacing(F32 spacing)
	{
		mLayoutSapcing = spacing;
	}

	void VerticalLayout::AddChild(WidgetPtr widget)
	{
		Widget::Add(widget);

		mChildren.push_back(widget);
		mIsNeedLayout = true;
	}

	void VerticalLayout::RemoveChild(WidgetPtr widget)
	{
		auto it = std::find(mChildren.begin(), mChildren.end(), widget);
		if (it != mChildren.end())
		{
			Widget::Remove(widget);

			mChildren.erase(it);
			mIsNeedLayout = true;
		}
	}

	void VerticalLayout::RemoveChild(U32 index)
	{
		if (index >= mChildren.size()) {
			return;
		}

		auto it = std::next(mChildren.begin(), index);
		if (it == mChildren.end()) {
			return;
		}

		Widget::Remove(*it);

		mChildren.erase(it);
		mIsNeedLayout = true;
	}

	U32 VerticalLayout::GetChildrenCount() const
	{
		return mChildren.size();
	}

	std::vector<WidgetPtr>& VerticalLayout::GetChildren()
	{
		return mChildren;
	}

	void VerticalLayout::UpdateLayoutImpl(const Rect& destRect)
	{
		if (mChildren.empty())
		{
			SetSize({ GetWidth(), 0.0f });
			mIsNeedLayout = false;
			return;
		}

		// calculate height
		F32 newHeight = 0.0f;
		for (auto& child : mChildren) {
			newHeight += child->GetHeight();
		}
		newHeight += (mLayoutSapcing * (F32)std::max(size_t(0), (mChildren.size() - 1)));
		newHeight += mMargin.top + mMargin.bottom;

		if (newHeight != GetHeight()) {
			SetSize({ GetWidth(), newHeight });
		}

		// children layout
		F32 currentPosY = mMargin.top;
		for (auto& child : mChildren) 
		{
			F32x2 pos = F32x2(mMargin.left, currentPosY);
			U32 stick = child->GetStick();
			if (stick & WidgetAlignment::HCenter)
			{
				pos[0] += (GetWidth() - (mMargin.left + mMargin.right) - child->GetWidth()) * 0.5f;
			}
			else if (stick & WidgetAlignment::Right)
			{
				pos[0] += (GetWidth() - (mMargin.left + mMargin.right) - child->GetWidth());
			}

			child->SetPos(pos);
			currentPosY += child->GetHeight() + mLayoutSapcing;
		}

		mIsNeedLayout = false;
	}

}
}