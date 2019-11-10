#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"

namespace Cjing3D {

class PipelineStateInfoManager
{
public:
	PipelineStateInfoManager(Renderer& renderer);
	~PipelineStateInfoManager();

	PipelineStateInfo GetPipelineStateInfo(ShaderType shaderType, MaterialComponent& material);
	PipelineStateInfo GetImagePipelineStateInfo();

	void SetupPipelineStateInfos();

private:
	Renderer & mRenderer;
};

}