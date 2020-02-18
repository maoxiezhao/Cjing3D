#pragma once

#include "gui\widgets.h"

namespace Cjing3D {
namespace Gui
{
	class Panel : public Widget
	{
	public:
		Panel(GUIStage& stage, const StringID& name = StringID::EMPTY);
		virtual ~Panel();

		virtual void InitProperties(tinyxml2::XMLElement& element);
		virtual void RenderImpl(const Rect& destRect);

		virtual WidgetType GetSelfWidgetType() const {
			return Panel::GetWidgetType();
		}

		static WidgetType GetWidgetType() {
			return WidgetType::WidgetType_Panel;
		}
	};
}
}