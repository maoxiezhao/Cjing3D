#include "widgetManager.h"
#include "guiStage.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{
	// xml parse functions
	namespace {
		std::shared_ptr<tinyxml2::XMLDocument> LoadCachedXML(const std::string& path)
		{
			if (FileData::IsFileExists(path) == false) {
				return nullptr;
			}

			std::string buffer = FileData::ReadFile(path);
			if (buffer.empty()) {
				return nullptr;
			}

			std::shared_ptr<tinyxml2::XMLDocument> xml = std::make_shared<tinyxml2::XMLDocument>();
			xml->Parse(buffer.c_str());
			
			if (xml->ErrorID() != tinyxml2::XML_SUCCESS) {
				return nullptr;
			}

			return xml;
		}
	}

	WidgetManager::WidgetManager(GUIStage& guiStage):
		mGUIStage(guiStage),
		mGUIFactory(nullptr)
	{
	}

	WidgetManager::~WidgetManager()
	{
	}

	void WidgetManager::Initialize()
	{
		mGUIFactory = std::make_unique<GUIFactory>(mGUIStage);

		mGUIFactory->RegisterWidgetType<Widget>();
	}

	void WidgetManager::Uninitialize()
	{
	}

	WidgetPtr WidgetManager::CreateWidgetFromXML(const std::string& name)
	{
		std::shared_ptr<tinyxml2::XMLDocument> xmlPtr = LoadCachedXML(name);
		if (xmlPtr == nullptr) {
			return nullptr;
		}

		auto rootNode = xmlPtr->FirstChildElement();
		if (rootNode == nullptr || !IsAvailableWidget(rootNode->Name()) ) {
			return nullptr;
		}

		// 首先处理templates
		LoadWidgetTemplateFromXMLElement(*rootNode);

		// 然后创建对应的widget
		WidgetPtr newWidget = CreateWidgetFromXMLElement(*rootNode);

		return newWidget;
	}
	bool WidgetManager::IsAvailableWidget(const std::string& name) const
	{
		auto& registeredTypes = mGUIFactory->GetRegisteredWidgetTypes();
		return registeredTypes.find(StringID(name)) != registeredTypes.end();
	}

	WidgetPtr WidgetManager::CreateWidgetFromXMLElement(tinyxml2::XMLElement& element)
	{
		return WidgetPtr();
	}

	void WidgetManager::LoadWidgetTemplateFromXMLElement(tinyxml2::XMLElement& element)
	{
	}

	WidgetPtr WidgetManager::CreateWidget(WidgetType type, const StringID& name)
	{
		return WidgetPtr();
	}

	WidgetPtr WidgetManager::CreateWidgetFromTemplate(const StringID& tempalteName)
	{
		return WidgetPtr();
	}
}