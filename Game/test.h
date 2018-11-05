#pragma once

#include "engine.h"

using namespace Cjing3D;

class TestGameComponent : public GameComponent
{
public:
	TestGameComponent() {};
	virtual ~TestGameComponent() {};

protected:
	virtual void BeforeInitializeImpl();
	virtual void AfterInitializeImpl();
	virtual void UpdateImpl(EngineTime time);
	virtual void UninitializeImpl();
};

class TestRenderableComponent : public RenderableComponent3D
{
public:

};