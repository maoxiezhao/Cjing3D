#pragma once

#include "gui\guiInclude.h"
#include "gui\widgets.h"
#include "gui\guiFactory.h"

#include <map>
#include <set>

namespace Cjing3D
{
	class GUIStage;

	// WidgetManager 负责Widget的实例创建，包括从定义中创建和从xml文件中创建
	class WidgetManager
	{
	public:
		using XMLDocumentPtr = std::shared_ptr<tinyxml2::XMLDocument>;

		WidgetManager(GUIStage& guiStage);
		~WidgetManager();

		void Initialize();
		void Uninitialize();

		WidgetPtr CreateWidgetFromXMLFile(const std::string& name, LuaRef scriptHandler = LuaRef::NULL_REF);
		WidgetPtr CreateWidget(const StringID& type, const StringID& name);
		WidgetPtr CreateWidgetFromDefinition(const StringID& definition, std::vector<WidgetPtr>* newWidgets = nullptr);
		WidgetPtr CreateWidgetFromXMLElement(WidgetPtr parent, tinyxml2::XMLElement& element, std::vector<WidgetPtr>* newWidgets = nullptr);

		bool IsAvailableWidget(const std::string& name) const;

	private:
		XMLDocumentPtr LoadCachedXML(const std::string& path);
		void UnloadCachedXML(const std::string& path);
		void LoadDefinition(tinyxml2::XMLElement& element);
		void UnloadDefinition(tinyxml2::XMLElement& element);

		void LoadWidgetProperties(Widget& widget, tinyxml2::XMLElement& element);
		void ParseWidgetEventHandlers(Widget& widget, tinyxml2::XMLElement& element);

	private:
		GUIStage& mGUIStage;

		std::map<StringID, XMLDocumentPtr> mCachedXMLDocument;
		std::map<StringID, tinyxml2::XMLElement*> mDefinitionMap;
		std::unique_ptr<GUIFactory> mGUIFactory;
	};
} 