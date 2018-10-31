#pragma once

#include "renderer\renderableCommon.h"
#include "core\gameSystem.hpp"

namespace Cjing3D
{

class GraphicsDevice;
class ShaderLib;
class StateManager;
class ResourceManager;

class Renderer : public GameSystem
{
public:
	Renderer(GameContext& gameContext);
	~Renderer();

	void Initialize(GraphicsDevice* device);
	void Uninitialize();
	void Present();
	
	GraphicsDevice& GetDevice();
	ResourceManager& GetResourceManager();

private:
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;

	bool mIsInitialized;
};

}