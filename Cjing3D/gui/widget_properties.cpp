#include "widget_properties.h"
#include "widgets.h"

namespace Cjing3D
{
	namespace {
		bool StringToBool(tinyxml2::XMLElement& setting, bool defaultValue = true)
		{
			return setting.BoolText(defaultValue);
		}

		Rect StringToRect(tinyxml2::XMLElement& setting, Rect defaultValue)
		{
			const std::string string = setting.GetText();
			if (string.empty()) {
				return defaultValue;
			}

			F32 x, y, w, h = 0.0f;
			sscanf_s(string.data(), "%f %f %f %f", &x, &y, &w, &h);

			return Rect({ x, y }, { w, h });
		};
	
		F32x2 StringToF32x2(tinyxml2::XMLElement& setting, F32x2 defaultValue = { 0.0f, 0.0f })
		{
			const std::string string = setting.GetText();
			if (string.empty()) {
				return defaultValue;
			}

			F32 x, y = 0.0f;
			sscanf_s(string.data(), "%f %f", &x, &y);

			return {x, y};
		}
	}

	WidgetPropertiesInitializer::WidgetPropertiesInitializer()
	{
	}

	WidgetPropertiesInitializer::~WidgetPropertiesInitializer()
	{
	}

	void WidgetPropertiesInitializer::InitProperties(Widget& widget, tinyxml2::XMLElement& element)
	{
		auto setting = element.FirstChildElement("Visible");
		if (setting != nullptr) {
			widget.SetVisible(StringToBool(*setting));
		}

		setting = element.FirstChildElement("Area");
		if (setting != nullptr) {
			widget.SetArea(StringToRect(*setting, Rect()));
		}

		setting = element.FirstChildElement("Size");
		if (setting != nullptr) {
			widget.SetSize(StringToF32x2(*setting));
		}

		setting = element.FirstChildElement("Position");
		if (setting != nullptr) {
			widget.SetPos(StringToF32x2(*setting));
		}
	}

	void WidgetPropertiesInitializer::ParseEventHandlers(Widget& widget, tinyxml2::XMLElement& element)
	{
		auto childNode = element.FirstChildElement();
		while (childNode != nullptr)
		{
			StringID eventType = StringID(childNode->Name());
			auto it = GUIScriptEventHandlers::registeredScriptEventHandlers.find(eventType);
			if (it != GUIScriptEventHandlers::registeredScriptEventHandlers.end())
			{
				std::string handler = childNode->GetText();
				if (!handler.empty()) {
					widget.AddScriptEventHandler(eventType, handler);
				}
			}

			childNode = childNode->NextSiblingElement();
		}
	}
}