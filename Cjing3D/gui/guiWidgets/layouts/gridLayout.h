#pragma once

#include "gui\guiWidgets\layouts\layout.h"

namespace Cjing3D {
	namespace Gui {

		class SimpleGridLayout : public Layout
		{
		public:
			enum GridLayoutMode
			{
				GridLayoutMode_Adaptive,
				GridLayoutMode_FactorFixed,
			};

			SimpleGridLayout(
				AlignmentOrien orien, 
				AlignmentMode alignMode, 
				U32 itemCount,
				WidgetMargin margin = WidgetMargin(), 
				F32 spacing = 0.0f);

			virtual void UpdateLayout(Widget* widget);
			virtual F32x2 CalculateBestSize(const Widget* widget);

			void SetSpacing(F32 spacing) { mSpacing = spacing; }
			void SetMargin(const WidgetMargin& margin) { mMargin = margin; }
			void SetColAlignment(const std::vector<AlignmentMode>& alignments) { mAxisAlignment[0] = alignments; }
			void SetRowAlignment(const std::vector<AlignmentMode>& alignments) { mAxisAlignment[1] = alignments; }
			void SetDefaultColAlignment(AlignmentMode alignments) { mDefaultAxisAlignment[0] = alignments; }
			void SetDefaultRowAlignment(AlignmentMode alignments) { mDefaultAxisAlignment[1] = alignments; }

			AlignmentMode GetItemAlignment(AlignmentOrien axis, U32 index);

		private:
			void ComputeLayout(const Widget* widget, std::vector<F32>* grid);
			F32x2 CalculateBestSizeImpl(const Widget* widget, std::vector<F32>* grid);

		private:
			U32 mGridItemCount = 0;
			WidgetMargin mMargin;
			F32x2 mSpacing;
			std::vector<F32> mGridItemFactors;

			GridLayoutMode mLayoutMode = GridLayoutMode_Adaptive;
			AlignmentOrien mAlignOri;
			AlignmentMode mDefaultAxisAlignment[2];
			std::vector<AlignmentMode> mAxisAlignment[2];
		};
	}
}