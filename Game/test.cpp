#include "test.h"

TestGameComponent::TestGameComponent()
{
}

TestGameComponent::~TestGameComponent()
{
}

void TestGameComponent::InitializeImpl()
{

	this->RequestRenderableComponent(std::make_shared<TestRenderableComponent>());
}

void TestGameComponent::UpdateImpl(EngineTime time)
{
}

void TestGameComponent::UninitializeImpl()
{


}
