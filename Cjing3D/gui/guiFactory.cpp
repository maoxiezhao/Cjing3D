#include "guiFactory.h"
#include "gui\guiStage.h"

namespace Cjing3D
{
	GUIFactory::GUIFactory(GUIStage& guiStage) :
		mGUIStage(guiStage)
	{
	}

	GUIFactory::~GUIFactory()
	{
	}

	void GUIFactory::RegisterWidgetType(const StringID& typeName, BaseWidgetCreatorPtr creator)
	{
		mRegisteredWidgetCreators[typeName] = creator;
		mRegisteredWidgetTypes.insert(typeName);
	}
}