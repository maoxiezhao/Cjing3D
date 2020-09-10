#include "guiFactory.h"
#include "gui\guiStage.h"

namespace Cjing3D
{
namespace Gui
{

	GUIFactory::GUIFactory(GUIStage& guiStage) :
		mGUIStage(guiStage)
	{
	}

	GUIFactory::~GUIFactory()
	{
	}

	void GUIFactory::RegisterWidgetType(const StringID& typeName, WidgetCreatorPtr creator)
	{
		mRegisteredWidgetCreators[typeName] = creator;
		mRegisteredWidgetTypes.insert(typeName);
	}

	WidgetPtr GUIFactory::CreateWidget(const StringID& type, const StringID& name)
	{
		auto it = mRegisteredWidgetCreators.find(type);
		if (it == mRegisteredWidgetCreators.end())
		{
			Logger::Warning("[GUIFactory] Invalid widget type:" + type.GetString() + " name:" + name.GetString());
			return nullptr;
		}

		return it->second->Create(name);
	}

	WidgetPtr DefaultWidgetCreator::Create(const StringID& name)
	{
		return std::make_shared<Widget>(mGUIStage, name);
	}
}
}