#include "guiScheme.h"

namespace Cjing3D
{
namespace Gui
{
#define REGISTER_SCHEME_NAME(name) StringID name = StringID(#name)
	// window
	REGISTER_SCHEME_NAME(GUIScheme::WindowBackgound);
	REGISTER_SCHEME_NAME(GUIScheme::WindowTitleText);
	REGISTER_SCHEME_NAME(GUIScheme::WindowBackgoundFocused);
	REGISTER_SCHEME_NAME(GUIScheme::WindowTitleTextFocused);
	REGISTER_SCHEME_NAME(GUIScheme::WindowHeadBackground);
	// panel
	REGISTER_SCHEME_NAME(GUIScheme::PanelBackground);
	// popup
	REGISTER_SCHEME_NAME(GUIScheme::PopupPanelBackground);
	// button
	REGISTER_SCHEME_NAME(GUIScheme::ButtonBackgroundBase);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonBackgroundClick);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonBackgroundHovered);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonTextBase);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonTextClick);
	REGISTER_SCHEME_NAME(GUIScheme::ButtonTextHovered);
	// list item
	REGISTER_SCHEME_NAME(GUIScheme::ListItemBackgroundHovered);
	REGISTER_SCHEME_NAME(GUIScheme::ListItemBackgroundPressed);
	REGISTER_SCHEME_NAME(GUIScheme::ListItemBackgroundSelected);
	REGISTER_SCHEME_NAME(GUIScheme::ListPanelBackground);
	// check box
	REGISTER_SCHEME_NAME(GUIScheme::CheckBoxBackground);
	REGISTER_SCHEME_NAME(GUIScheme::CheckBoxBackgroundOn);
	// scroll
	REGISTER_SCHEME_NAME(GUIScheme::ScrollBarBase);
	REGISTER_SCHEME_NAME(GUIScheme::ScrollSliderBase);
	REGISTER_SCHEME_NAME(GUIScheme::ScrollSliderHovered);
	// text
	REGISTER_SCHEME_NAME(GUIScheme::BaseTextColor);
	// text field
	REGISTER_SCHEME_NAME(GUIScheme::TextFieldBorder);
	REGISTER_SCHEME_NAME(GUIScheme::TextFieldBackground);
	

#undef  REGISTER_SCHEME_NAME

	void GUIScheme::RegisterColor(const StringID& name, const Color4& color)
	{
		mRegisteredColor[name] = color;
	}

	void GUIScheme::RegisterImg(const StringID& name)
	{
		// TODO
	}

	Color4 GUIScheme::GetColor(const StringID& name) const
	{
		auto it = mRegisteredColor.find(name);
		if (it != mRegisteredColor.end())
		{
			return it->second;
		}
		return Color4::White();
	}

	void GUIScheme::InitDefaultScheme()
	{
		// window
		RegisterColor(GUIScheme::WindowBackgound, Color4(43, 43, 43, 230));
		RegisterColor(GUIScheme::WindowTitleText, Color4(220, 220, 220, 160));
		RegisterColor(GUIScheme::WindowBackgoundFocused, Color4(45, 45, 45, 230));
		RegisterColor(GUIScheme::WindowTitleTextFocused, Color4(255, 255, 255, 190));
		RegisterColor(GUIScheme::WindowHeadBackground, Color4(74, 74, 74, 255));
		// panel
		RegisterColor(GUIScheme::PanelBackground, Color4(45, 45, 48, 225));
		// popup
		RegisterColor(GUIScheme::PopupPanelBackground, Color4(85, 85, 88, 225));	
		// button
		RegisterColor(GUIScheme::ButtonBackgroundBase, Color4(58, 58, 58, 255));
		RegisterColor(GUIScheme::ButtonBackgroundClick, Color4(31, 31, 31, 255));
		RegisterColor(GUIScheme::ButtonBackgroundHovered, Color4(64, 64, 64, 255));
		RegisterColor(GUIScheme::ButtonTextBase, Color4(255, 255, 255, 160));
		RegisterColor(GUIScheme::ButtonTextClick, Color4(255, 255, 255, 160));
		RegisterColor(GUIScheme::ButtonTextHovered, Color4(255, 255, 255, 160));
		// list item
		RegisterColor(GUIScheme::ListItemBackgroundHovered, Color4(68, 67, 65, 255));
		RegisterColor(GUIScheme::ListItemBackgroundPressed, Color4(0, 123, 182, 255));
		RegisterColor(GUIScheme::ListItemBackgroundSelected, Color4(110, 108, 109, 255));
		RegisterColor(GUIScheme::ListPanelBackground, Color4(40, 40, 90, 255));
		// check box
		RegisterColor(GUIScheme::CheckBoxBackground, Color4(120, 120, 100, 255));
		RegisterColor(GUIScheme::CheckBoxBackgroundOn, Color4(200, 200, 189, 255));
		// scroll
		RegisterColor(GUIScheme::ScrollBarBase, Color4(120, 120, 120, 150));
		RegisterColor(GUIScheme::ScrollSliderBase, Color4(220, 220, 220, 150));
		RegisterColor(GUIScheme::ScrollSliderHovered, Color4(240, 240, 240, 150));
		// text
		RegisterColor(GUIScheme::BaseTextColor, Color4(255, 255, 255, 200));
		// text field
		RegisterColor(GUIScheme::TextFieldBorder, Color4(194, 190, 191, 255));
		RegisterColor(GUIScheme::TextFieldBackground, Color4(122, 120, 121, 255));
	}


}
}