#include "test.h"

TestGameComponent::TestGameComponent()
{
}

TestGameComponent::~TestGameComponent()
{
}

void TestGameComponent::BeforeInitializeImpl()
{
	this->RequestRenderableComponent(std::make_shared<TestRenderableComponent>());
}

void TestGameComponent::AfterInitializeImpl()
{

}

void TestGameComponent::UpdateImpl(EngineTime time)
{
}

void TestGameComponent::UninitializeImpl()
{


}
