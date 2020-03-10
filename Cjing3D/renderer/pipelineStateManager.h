#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D {

struct NormalRenderParams
{
	RenderPassType renderPassType;

	NormalRenderParams() :
		renderPassType(RenderPassType_Forward)
	{}
};

// TODO:Ӧ��bufferManager��ShaderLib����PipelineState���ϣ���Image��Render������
// ��ʱ����ͨ��pipelineState��imageState����һ��
class PipelineStateManager
{
public:
	PipelineStateManager(Renderer& renderer);
	~PipelineStateManager();

	PipelineState GetNormalPipelineState(RenderPassType renderPassType, MaterialComponent& material);
	PipelineState GetImagePipelineState(RenderImage::ImageParams params);

	void Initialize();
	void Uninitialize();
	
	U32 RegisterPipelineState(PipelineState pipelineState);
	void RegisterPipelineState(RenderImage::ImageParams params, PipelineState state);
	void RegisterPipelineState(NormalRenderParams params, PipelineState state);

private:
	void SetupPipelineStates();
	void SetupImagePipelineStates();

	Renderer & mRenderer;

	std::map<U32, U32> mImagePipelineStateIndexMap;
	std::map<U32, U32> mNormalPipelineStateIndexMap;
	std::vector<PipelineState> mPipelineStates;	
};

}