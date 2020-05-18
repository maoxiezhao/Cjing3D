#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D {

enum PipelineStateID {
	PipelineStateID_SkyRendering = 0,
};

struct NormalRenderParams
{
	RenderPassType renderPassType;

	NormalRenderParams() :
		renderPassType(RenderPassType_Forward)
	{}
};

// TODO:应将bufferManager、ShaderLib等与PipelineState整合，以Image和Render来区分
// 暂时将普通的pipelineState和imageState放在一起
class PipelineStateManager
{
public:
	PipelineStateManager(Renderer& renderer);
	~PipelineStateManager();

	PipelineState GetNormalPipelineState(RenderPassType renderPassType, MaterialComponent& material);
	PipelineState GetImagePipelineState(RenderImage::ImageParams params);
	PipelineState GetPipelineStateByID(PipelineStateID stateID);

	void Initialize();
	void Uninitialize();

	void RegisterPipelineState(RenderImage::ImageParams params, PipelineStateDesc desc);
	void RegisterPipelineState(NormalRenderParams params, PipelineStateDesc desc);
	void RegisterPipelineState(PipelineStateID stateID, PipelineStateDesc desc);

private:
	U32 RegisterPipelineState(PipelineStateDesc desc);

	void SetupNormalPipelineStates();
	void SetupImagePipelineStates();
	void SetupPipelineStateIDs();

	Renderer & mRenderer;

	std::vector<PipelineState> mPipelineStates;	

	std::map<U32, U32> mImagePipelineStateIndexMap;
	std::map<U32, U32> mNormalPipelineStateIndexMap;
	std::map<U32, U32> mPipelineStateIDMap;
};

}