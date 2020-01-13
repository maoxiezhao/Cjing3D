#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D {

// TODO:应将bufferManager、ShaderLib等与PipelineStateInfo整合，以Image和Render来区分

class PipelineStateInfoManager
{
public:
	PipelineStateInfoManager(Renderer& renderer);
	~PipelineStateInfoManager();

	PipelineStateInfo GetPipelineStateInfo(ShaderType shaderType, MaterialComponent& material);
	PipelineStateInfo GetImagePipelineStateInfo(RenderImage::ImageParams params);

	void SetupPipelineStateInfos();

private:
	Renderer & mRenderer;
};

}