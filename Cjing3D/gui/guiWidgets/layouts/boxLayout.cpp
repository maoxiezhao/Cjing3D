#include "boxLayout.h"

namespace Cjing3D
{
namespace Gui
{
	BoxLayout::BoxLayout(AlignmentOrien orien, AlignmentMode alignMode, WidgetMargin margin, F32 spacing) :
		mAlignOri(orien),
		mAlignMode(alignMode),
		mMargin(margin),
		mSpacing(spacing)
	{
	}

	void BoxLayout::UpdateLayout(Widget* widget)
	{
		F32x2 parentSize = widget->GetAvailableSize();
		F32x2 marginSize;
		switch (mAlignOri)
		{
		case AlignmentOrien_Horizontal:
			marginSize = { mMargin.left, mMargin.right };
			break;
		case AlignmentOrien_Vertical:
			marginSize = { mMargin.top, mMargin.bottom };
			break;
		}

		bool first = true;
		int axis1 = (int)mAlignOri, axis2 = ((int)mAlignOri + 1) % 2;
		F32 currentPosition = marginSize[axis1];
		for (auto child : widget->GetChildren())
		{
			if (!child->IsVisible()) {
				continue;
			}

			if (first) {
				first = false;
			}
			else {
				currentPosition += mSpacing;
			}

			F32x2 childTargetSize = child->GetBestSize();
			F32x2 targetPos;
			targetPos[axis1] = currentPosition;
			switch (mAlignMode)
			{
			case AlignmentMode_Begin:
				targetPos[axis2] += marginSize[0];
				break;
			case AlignmentMode_Center:
				targetPos[axis2] += (parentSize[axis2] - childTargetSize[axis2]) * 0.5f;
				break;
			case AlignmentMode_End:
				targetPos[axis2] += (parentSize[axis2] - childTargetSize[axis2]) - marginSize[1];
				break;
			case AlignmentMode_Fill:
				targetPos[axis2] += marginSize[0];
				childTargetSize[axis2] = child->GetFixedSize()[axis2] ? 
					child->GetFixedSize()[axis2] : 
					(parentSize[axis2] - marginSize[0] - marginSize[1]);
				break;
			}

			child->SetPos(targetPos);
			child->SetSize(childTargetSize);
			child->UpdateLayout();

			currentPosition += childTargetSize[axis1];
		}
	}

	F32x2 BoxLayout::CalculateBestSize(const Widget* widget)
	{
		F32x2 marginSize = {mMargin.left + mMargin.right, mMargin.top + mMargin.bottom };
		F32x2 bestSize = marginSize;
		bool first = true;
		int axis1 = (int)mAlignOri, axis2 = ((int)mAlignOri + 1) % 2;
		for (auto child : widget->GetChildren()) 
		{
			if (!child->IsVisible()) {
				continue;
			}

			if (first) {
				first = false;
			}
			else {
				bestSize[axis1] += mSpacing;
			}

			F32x2 targetSize = child->GetBestSize();
			bestSize[axis1] += targetSize[axis1];
			bestSize[axis2] = std::max(bestSize[axis2], targetSize[axis2] + marginSize[axis2]);
			first = false;
		}
		return bestSize;
	}

}
}