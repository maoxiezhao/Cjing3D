#include "gridLayout.h"

#include <algorithm>
#include <numeric>

namespace Cjing3D
{
namespace Gui
{
	SimpleGridLayout::SimpleGridLayout(
			AlignmentOrien orien,
			AlignmentMode alignMode, 
			U32 itemCount, 
			WidgetMargin margin, 
			F32 spacing) :
		mAlignOri(orien),
		mGridItemCount(itemCount),
		mMargin(margin),
		mSpacing({ spacing, spacing })
	{
		mDefaultAxisAlignment[0] = mDefaultAxisAlignment[1] = alignMode;
		mGridItemFactors.resize(itemCount, 1.0f);
	}

	void SimpleGridLayout::UpdateLayout(Widget* widget)
	{
		F32x2 parentSize = widget->GetAvailableSize();
		std::vector<F32> axisGridSize[2];
		ComputeLayout(widget, axisGridSize);

		// strech size
		F32x2 gridSize = CalculateBestSizeImpl(widget, axisGridSize);
		for (int i = 0; i < 2; i++)
		{
			if (gridSize[i] < parentSize[i])
			{
				F32 gap = (parentSize[i] - gridSize[i]) / (F32)axisGridSize[i].size();
				for (int index = 0; index < axisGridSize[i].size(); index++) {
					axisGridSize[i][index] += gap;
				}
			}
		}


		std::vector<WidgetPtr> visibleWidgets;
		for (auto child : widget->GetChildren()) {
			if (child->IsVisible()) {
				visibleWidgets.push_back(child);
			}
		}

		// place children
		int axis1 = (int)mAlignOri, axis2 = ((int)mAlignOri + 1) % 2;
		F32x2 startPos = F32x2(mMargin.left, mMargin.top) + widget->GetLayoutOffset();
		F32x2 currentPos = startPos;

		size_t currentChildIndex = 0;
		for (int axis2Index = 0; axis2Index < axisGridSize[1].size(); axis2Index++)
		{
			currentPos[axis1] = startPos[axis1];
			for (int axis1Index = 0; axis1Index < axisGridSize[0].size(); axis1Index++)
			{
				if (currentChildIndex >= visibleWidgets.size()) {
					break;
				}
				WidgetPtr child = visibleWidgets[currentChildIndex++];
				F32x2 bestSize = child->GetBestSize();

				F32x2 childPos = currentPos;
				// x,y
				for (int i = 0; i < 2; i++)
				{
					int axis = (axis1 + i) % 2;
					int itemIndex = (i == 0) ? axis1Index : axis2Index;
					AlignmentMode align = GetItemAlignment(AlignmentOrien(axis), itemIndex);
					switch (align)
					{
					case Cjing3D::Gui::AlignmentMode_Begin:
						break;
					case Cjing3D::Gui::AlignmentMode_Center:
						childPos[axis] += (axisGridSize[axis][itemIndex] - bestSize[axis]) * 0.5f;
						break;
					case Cjing3D::Gui::AlignmentMode_End:
						childPos[axis] += (axisGridSize[axis][itemIndex] - bestSize[axis]);
						break;
					case Cjing3D::Gui::AlignmentMode_Fill:
						auto fixedSize = child->GetFixedSize();
						bestSize[axis] = fixedSize[axis] != 0.0f ? fixedSize[axis] : axisGridSize[axis][itemIndex];
						break;
					}
				}

				child->SetPos(childPos);
				child->SetSize(bestSize);
				child->UpdateLayout();
				currentPos[axis1] += axisGridSize[axis1][axis1Index] + mSpacing[axis1];
			}
			currentPos[axis2] += axisGridSize[axis2][axis2Index] + mSpacing[axis2];
		}
	}

	F32x2 SimpleGridLayout::CalculateBestSize(const Widget* widget)
	{
		std::vector<F32> axisGridSize[2];
		ComputeLayout(widget, axisGridSize);
		return CalculateBestSizeImpl(widget, axisGridSize);
	}

	AlignmentMode SimpleGridLayout::GetItemAlignment(AlignmentOrien axis, U32 index)
	{
		if (index < mAxisAlignment[axis].size()) {
			return mAxisAlignment[axis][index];
		}

		return mDefaultAxisAlignment[axis];
	}

	void SimpleGridLayout::ComputeLayout(const Widget* widget, std::vector<F32>* grid)
	{
		int axis1 = (int)mAlignOri, axis2 = ((int)mAlignOri + 1) % 2;
		std::vector<WidgetPtr> visibleWidgets;
		for (auto child : widget->GetChildren()) {
			if (child->IsVisible()) {
				visibleWidgets.push_back(child);
			}
		}

		U32 axis1ItemCount = mGridItemCount;
		U32 axis2ItemCount = (visibleWidgets.size() + mGridItemCount - 1) / mGridItemCount;

		grid[axis1].clear();
		grid[axis2].clear();
		grid[axis1].resize(axis1ItemCount, 0.0f);
		grid[axis2].resize(axis2ItemCount, 0.0f);

		F32 totalAxis1Size = 0.0f;
		int currentChildIndex = 0;
		for (int axis2Index = 0; axis2Index < axis2ItemCount; axis2Index++)
		{
			for (int axis1Index = 0; axis1Index < axis1ItemCount; axis1Index++)
			{
				if (currentChildIndex >= visibleWidgets.size()) {
					break;
				}
				WidgetPtr child = visibleWidgets[currentChildIndex++];
				F32x2 bestSize = child->GetBestSize();

				grid[axis1][axis1Index] = std::max(grid[axis1][axis1Index], bestSize[axis1]);
				grid[axis2][axis2Index] = std::max(grid[axis2][axis2Index], bestSize[axis2]);

				totalAxis1Size += bestSize[axis1];
			}
		}
	}

	F32x2 SimpleGridLayout::CalculateBestSizeImpl(const Widget* widget, std::vector<F32>* grid)
	{
		F32x2 marginSize = { mMargin.left + mMargin.right, mMargin.top + mMargin.bottom };
		F32x2 size = F32x2(
			marginSize[0] + std::accumulate(grid[0].begin(), grid[0].end(), 0.0f) + (F32)std::max((int)grid[0].size() - 1, 0) * mSpacing[0],
			marginSize[1] + std::accumulate(grid[1].begin(), grid[1].end(), 0.0f) + (F32)std::max((int)grid[1].size() - 1, 0) * mSpacing[1]
		);
		F32x2 offset = widget->GetLayoutOffset();
		return size + offset;
	}
}
}