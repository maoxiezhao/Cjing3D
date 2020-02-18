#include "Panel.h"

namespace Cjing3D {
namespace Gui
{
	Panel::Panel(GUIStage& stage, const StringID& name) :
		Widget(stage, name)
	{
	}

	Panel::~Panel()
	{
	}

	void Panel::InitProperties(tinyxml2::XMLElement& element)
	{
		Widget::InitProperties(element);
	}

	void Panel::RenderImpl(const Rect& destRect)
	{
		Widget::RenderImpl(destRect);
	}
}
}