#pragma once

#include "gui\guiInclude.h"
#include "gui\widgets.h"
#include "gui\guiFactory.h"

#include "utils\tinyxml2\tinyxml2.h"

#include <map>
#include <set>

namespace Cjing3D
{
	class GUIStage;

	class WidgetManager
	{
	public:
		using XMLElementPtr = std::shared_ptr<tinyxml2::XMLElement>;
		
		WidgetManager(GUIStage& guiStage);
		~WidgetManager();

		void Initialize();
		void Uninitialize();

		WidgetPtr CreateWidgetFromXML(const std::string& name);
		WidgetPtr CreateWidget(WidgetType type, const StringID& name);
		WidgetPtr CreateWidgetFromTemplate(const StringID& tempalteName);

		bool IsAvailableWidget(const std::string& name) const;

	private:
		WidgetPtr CreateWidgetFromXMLElement(tinyxml2::XMLElement& element);
		void LoadWidgetTemplateFromXMLElement(tinyxml2::XMLElement& element);

	private:
		GUIStage& mGUIStage;

		std::unique_ptr<GUIFactory> mGUIFactory;
		std::map<StringID, XMLElementPtr> mTemplateMap;
	};
} 