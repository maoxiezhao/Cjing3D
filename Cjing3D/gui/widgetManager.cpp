#include "widgetManager.h"
#include "guiStage.h"
#include "helper\fileSystem.h"

namespace Cjing3D
{
	const std::string xmlDataType = "data";
	const std::string xmlRootType = "root";
	const std::string xmlDefinitionsType = "definitions";
	const std::string xmlChildrenType = "children";

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

		mGUIFactory->RegisterWidgetType(StringID("root"), WidgetCreatorPtr(new DefaultWidgetCreator(mGUIStage)) );
		mGUIFactory->RegisterWidgetType<Widget>();
	}

	void WidgetManager::Uninitialize()
	{
		mCachedXMLDocument.clear();
		mDefinitionMap.clear();
	}

	WidgetPtr WidgetManager::CreateWidgetFromXMLFile(const std::string& name)
	{
		std::shared_ptr<tinyxml2::XMLDocument> xmlPtr = LoadCachedXML(name);
		if (xmlPtr == nullptr) {
			return nullptr;
		}

		// 根节点名字必须是data
		auto dataNode = xmlPtr->FirstChildElement();
		if (dataNode == nullptr &&
			dataNode->Name() != xmlRootType ) {
			return nullptr;
		}

		// 首先尝试从<definitions>读取所有definitions
		auto definitionsNode = dataNode->FirstChildElement(xmlDefinitionsType.c_str());
		if (definitionsNode != nullptr)
		{
			auto childNode = definitionsNode->FirstChildElement();
			while (childNode != nullptr)
			{
				LoadDefinition(*childNode);
				childNode = childNode->NextSiblingElement();
			}
		}

		// 然后尝试从<root>创建widget
		WidgetPtr newWidget = nullptr;
		auto rootNode = dataNode->FirstChildElement(xmlRootType.c_str());
		if (rootNode != nullptr) {
			newWidget = CreateWidgetFromXMLElement(nullptr, *rootNode);
		}
		 
		return newWidget;
	}

	WidgetPtr WidgetManager::CreateWidgetFromXMLElement(WidgetPtr parent, tinyxml2::XMLElement& element)
	{
		// 分析element node, 创建对应类型的widget，可以通过use_definition来使用定义
		std::string type = element.Name();
		if (IsAvailableWidget(type) == false) {
			return nullptr;
		}

		StringID name = StringID(element.StringAttribute("name"));
		std::string useDef = element.StringAttribute("use_definition");

		auto CreateWidgetFunc = [&, type, name, useDef]()
		{
			WidgetPtr widget = nullptr;
			if (!useDef.empty()) {
				widget = CreateWidgetFromDefinition(StringID(useDef));

				// 仅当definition有效，才会直接返回，不然按默认widget创建
				if (widget != nullptr) {
					return widget;
				}
			}
			
			widget = CreateWidget(StringID(type), name);
	
			return widget;
		};

		WidgetPtr newWidget = nullptr;
		if (parent != nullptr)
		{
			newWidget = parent->Find(name);
			if (newWidget != nullptr) {
				return nullptr;
			}

			newWidget = CreateWidgetFunc();
			if (newWidget == nullptr) {
				return nullptr;
			}

			parent->Add(newWidget);
		}
		else
		{
			newWidget = CreateWidgetFunc();
			if (newWidget == nullptr) {
				return nullptr;
			}
		}

		newWidget->SetName(name);

		LoadWidgetProperties(*newWidget, element);

		// create widget children
		auto  childrenNodes = element.FirstChildElement(xmlChildrenType.c_str());
		if (childrenNodes != nullptr)
		{
			auto childNode = childrenNodes->FirstChildElement();
			while (childNode != nullptr)
			{
				CreateWidgetFromXMLElement(newWidget, *childNode);
				childNode = childNode->NextSiblingElement();
			}
		}

		return newWidget;
	}

	void WidgetManager::LoadWidgetProperties(Widget& widget, tinyxml2::XMLElement& element)
	{

	}

	WidgetPtr WidgetManager::CreateWidget(const StringID& type, const StringID& name)
	{
		return mGUIFactory->CreateWidget(type, name);
	}

	WidgetPtr WidgetManager::CreateWidgetFromDefinition(const StringID& definition)
	{
		auto it = mDefinitionMap.find(definition);
		if (it == mDefinitionMap.end())
		{
			Logger::Warning("Invalid ui xml definition:" + definition.GetString());
			return nullptr;
		}

		return CreateWidgetFromXMLElement(nullptr, *it->second);
	}

	bool WidgetManager::IsAvailableWidget(const std::string& name) const
	{
		auto& registeredTypes = mGUIFactory->GetRegisteredWidgetTypes();
		return registeredTypes.find(StringID(name)) != registeredTypes.end();
	}

	std::shared_ptr<tinyxml2::XMLDocument> WidgetManager::LoadCachedXML(const std::string& path)
	{
		auto it = mCachedXMLDocument.find(StringID(path));
		if (it != mCachedXMLDocument.end()) {
			return it->second;
		}

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

		mCachedXMLDocument[StringID(path)] = xml;

		return xml;
	}

	void WidgetManager::UnloadCachedXML(const std::string& path)
	{
		auto it = mCachedXMLDocument.find(StringID(path));
		if (it == mCachedXMLDocument.end()) {
			return;
		}

		XMLDocumentPtr xmlPtr = it->second;

		auto dataNode = xmlPtr->FirstChildElement();
		if (dataNode == nullptr &&
			dataNode->Name() != xmlRootType) {
			return;
		}

		auto definitionsNode = dataNode->FirstChildElement(xmlDefinitionsType.c_str());
		if (definitionsNode != nullptr)
		{
			auto childNode = definitionsNode->FirstChildElement();
			while (childNode != nullptr)
			{
				UnloadDefinition(*childNode);
				childNode = childNode->NextSiblingElement();
			}
		}
	}

	void WidgetManager::LoadDefinition(tinyxml2::XMLElement& element)
	{
		// TODO
		StringID name = StringID(element.StringAttribute("name"));
		if (name != StringID::EMPTY) {
			mDefinitionMap[name] = &element;
		}
	}

	void WidgetManager::UnloadDefinition(tinyxml2::XMLElement& element)
	{
		StringID name = StringID(element.StringAttribute("name"));
		if (name != StringID::EMPTY) {
			mDefinitionMap.erase(name);
		}
	}
}