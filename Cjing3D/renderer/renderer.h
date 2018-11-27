#pragma once

#include "renderer\renderableCommon.h"
#include "core\subSystem.hpp"
#include "world\actor.h"

#include "renderer\pass\forwardPass.h"

#include <unordered_map>

namespace Cjing3D
{

class GraphicsDevice;
class ShaderLib;
class StateManager;
class ResourceManager;

enum RenderableType
{
	RenderableType_Opaque,
	RenderableType_Transparent
};

class Renderer : public SubSystem
{
public:
	Renderer(SystemContext& gameContext);
	~Renderer();

	void Initialize(GraphicsDevice* device);
	void Update();
	void Uninitialize();
	void Present();
	
	GraphicsDevice& GetDevice();
	ResourceManager& GetResourceManager();

	/** Rendering setting */

private:
	void InitializePasses();
	void AccquireActors(std::vector<ActorPtr> actors);

private:
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ShaderLib> mShaderLib;
	std::unique_ptr<StateManager> mStateManager;

	bool mIsInitialized;

	std::unordered_map<RenderableType, ActorPtrArray> mRenderingActors;

	/** rendering pass */
	std::unique_ptr<ForwardPass> mForwardPass;
	
};

}