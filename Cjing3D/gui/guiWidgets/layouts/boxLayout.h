#pragma once

#include "gui\guiWidgets\layouts\layout.h"

namespace Cjing3D {
namespace Gui {

	class BoxLayout : public Layout
	{
	public:
		BoxLayout(AlignmentOrien orien, AlignmentMode alignMode, WidgetMargin margin = WidgetMargin(), F32 spacing = 0.0f);

		virtual void UpdateLayout(Widget* widget);
		virtual F32x2 CalculateBestSize(const Widget* widget);

		void SetSpacing(F32 spacing) { mSpacing = spacing; }
		void SetMargin(const WidgetMargin& margin) { mMargin = margin; }

	private:
		AlignmentOrien mAlignOri;
		AlignmentMode mAlignMode;
		WidgetMargin mMargin;
		F32 mSpacing = 0.0f;
	};
}
}