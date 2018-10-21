#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\device.h"

namespace Cjing3D
{

class ResourceLoader;
class StateManager;

class Renderer
{
public:
	static Renderer& GetInstance() {
		static Renderer render;
		return render;
	}

	~Renderer();

	void Initialize(GraphicsDevice* device);
	void Uninitialize();
	void Present();
	
private:
	Renderer();

private:
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<ResourceLoader> mResourceLoader;
	std::unique_ptr<StateManager> mStateManager;

	bool mIsInitialized;
};

}