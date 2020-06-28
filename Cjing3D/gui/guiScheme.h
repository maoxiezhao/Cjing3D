#pragma once

#include "gui\guiInclude.h"

namespace Cjing3D {
namespace Gui {

	class GUIScheme
	{
	public:
		// window
		static StringID WindowBackgound;
		static StringID WindowTitleText;
		static StringID WindowBackgoundFocused;
		static StringID WindowTitleTextFocused;
		static StringID WindowHeadBackground;
		// panel
		static StringID PanelBackground;
		// button	
		static StringID ButtonBackgroundBase;
		static StringID ButtonBackgroundClick;
		static StringID ButtonBackgroundHovered;
		static StringID ButtonTextBase;
		static StringID ButtonTextClick;
		static StringID ButtonTextHovered;
		// list item
		static StringID ListItemBackgroundHovered;
		static StringID ListItemBackgroundPressed;
		static StringID ListItemBackgroundSelected;
		static StringID ListPanelBackground;
		// check box
		static StringID CheckBoxBackground;
		static StringID CheckBoxBackgroundOn;

		void InitDefaultScheme();
		void RegisterColor(const StringID& name, const Color4& color);
		void RegisterImg(const StringID& name);

		Color4 GetColor(const StringID& name)const;

	private:
		std::map<StringID, Color4> mRegisteredColor;
	};
}
}