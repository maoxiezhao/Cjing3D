#pragma once

#include "gui\guiInclude.h"
#include "gui\widgets.h"
#include "gui\guiFactory.h"

#include <map>
#include <set>

namespace Cjing3D
{
	class GUIStage;

	class WidgetManager
	{
	public:
		using XMLDocumentPtr = std::shared_ptr<tinyxml2::XMLDocument>;

		WidgetManager(GUIStage& guiStage);
		~WidgetManager();

		void Initialize();
		void Uninitialize();

		WidgetPtr CreateWidgetFromXMLFile(const std::string& name);
		WidgetPtr CreateWidget(const StringID& type, const StringID& name);
		WidgetPtr CreateWidgetFromDefinition(const StringID& definition);

		bool IsAvailableWidget(const std::string& name) const;

	private:
		XMLDocumentPtr LoadCachedXML(const std::string& path);
		void UnloadCachedXML(const std::string& path);
		void LoadDefinition(tinyxml2::XMLElement& element);
		void UnloadDefinition(tinyxml2::XMLElement& element);

		WidgetPtr CreateWidgetFromXMLElement(WidgetPtr parent, tinyxml2::XMLElement& element);
		void LoadWidgetProperties(Widget& widget, tinyxml2::XMLElement& element);

	private:
		GUIStage& mGUIStage;

		std::map<StringID, XMLDocumentPtr> mCachedXMLDocument;
		std::map<StringID, tinyxml2::XMLElement*> mDefinitionMap;
		std::unique_ptr<GUIFactory> mGUIFactory;
	};
} 