#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\renderResource.h"

namespace Cjing3D
{
class ResourceLoader
{
public:
	ResourceLoader(Renderer& renderer);

	void Initialize();
	void Uninitialize();

	void GetShader();
	void GetBuffer();

private:
	void LoadShader();
	void LoadBuffers();
	
private:
	Renderer & mRenderer;

};
}