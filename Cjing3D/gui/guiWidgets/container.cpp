#include "container.h"
#include "widgetInclude.h"

namespace Cjing3D
{
namespace Gui 
{
	Container::Container(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
	}

	void Container::SetSpacing(F32 spacing)
	{
		mSpacing = spacing;
		mNeedLayout = true;
	}

	void Container::SetMargin(const WidgetMargin& margin)
	{
		mMargin = margin;
		mNeedLayout = true;
	}

	void Container::SetLayoutDirty()
	{
		mNeedLayout = true;
	}

	void Container::Add(WidgetPtr node)
	{
		Widget::Add(node);
	
		mConnectionMap.Connect("OnBestSizeChanged", node->OnBestSizeChanged, std::bind(&Container::SetLayoutDirty, this));

		SetLayoutDirty();
	}

	void Container::Remove(const StringID& name)
	{
		Widget::Remove(name);

		mConnectionMap.Disconnect("OnBestSizeChanged");

		SetLayoutDirty();
	}

	void Container::Remove(WidgetPtr node)
	{
		Widget::Remove(node);

		SetLayoutDirty();
	}

	F32x2 Container::CalculateBestSize() const
	{
		if (mLayout != nullptr) {
			return mLayout->CalculateBestSize(this);
		}
		else {
			return Widget::CalculateBestSize();
		}
	}

	void Container::UpdateLayout()
	{
		if (!IsVisible()) {
			return;
		}

		for (auto& child : mChildren) {
			child->UpdateLayout();
		}

		if (!mNeedLayout) {
			return;
		}
		mNeedLayout = false;

		UpdateBestSize();

		if (mLayout != nullptr) {
			mLayout->UpdateLayout(this);
		}

		UpdateLayoutImpl(GetArea());
	}

	void Container::UpdateLayoutImpl(const Rect& destRect)
	{
	}

	void Container::UpdateChildAlignment(AlignmentOrien orien, WidgetPtr child, F32 offset, F32& pos, F32& size)
	{
		if (orien == AlignmentOrien_Horizontal)
		{
			F32 parentWidth = GetWidth();
			F32 targetX = pos;
			F32 targetWidth = size;

			// child horizontal placement
			AlignmentMode alignMode = child->GetHorizontalAlign();
			switch (alignMode)
			{
			case AlignmentMode_Center:
				targetX += (parentWidth - targetWidth) * 0.5f;
				break;
			case AlignmentMode_End:
				targetX += (parentWidth - targetWidth);
				break;
			case AlignmentMode_Fill:
				targetWidth = parentWidth - offset - (mMargin.left + mMargin.right);
				break;
			}

			pos = targetX;
			size = targetWidth;
		}
		else
		{
			F32 parentHeight = GetHeight();
			F32 targetY = pos;
			F32 targetHeight = size;

			// child horizontal placement
			AlignmentMode alignMode = child->GetHorizontalAlign();
			switch (alignMode)
			{
			case AlignmentMode_Center:
				targetY += (parentHeight - targetHeight) * 0.5f;
				break;
			case AlignmentMode_End:
				targetY += (parentHeight - targetHeight);
				break;
			case AlignmentMode_Fill:
				targetHeight = parentHeight - offset - (mMargin.top + mMargin.bottom);
				break;
			}

			pos = targetY;
			size = targetHeight;
		}
	}

}
}