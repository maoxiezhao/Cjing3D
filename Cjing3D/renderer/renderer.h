#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\device.h"

namespace Cjing3D
{

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

private:
	Renderer();

private:
	std::unique_ptr<GraphicsDevice> mGraphicsDevice;

	bool mIsInitialized;
};

}