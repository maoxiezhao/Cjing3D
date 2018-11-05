#include "test.h"
#include "resource\resourceManager.h"
#include "renderer\components\model.h"

void TestGameComponent::BeforeInitializeImpl()
{
	this->RequestRenderableComponent(std::make_shared<TestRenderableComponent>());
}

void TestGameComponent::AfterInitializeImpl()
{
	auto& resourceManager = GetGameContext().GetGameSystem<ResourceManager>();
	const auto& modelPath = resourceManager.GetStandardResourceDirectory(Resource_Model);
	
	resourceManager.GetOrCreate<Model>(modelPath + "cornell_box.obj");
}

void TestGameComponent::UpdateImpl(EngineTime time)
{
}

void TestGameComponent::UninitializeImpl()
{


}
