#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"
#include "renderer\RHI\rhiResource.h"
#include "renderer\RHI\rhiShader.h"
#include "renderer\paths\renderImage.h"

namespace Cjing3D {

enum PipelineStateID {
	PipelineStateID_SkyRendering = 0,
	PipelineStateID_GridHelper,
};

struct NormalRenderParams
{
	RenderPassType renderPassType = RenderPassType_Forward;
	BlendType blendType = BlendType_Opaque;
	bool enableAlphaTest = false;
};

// TODO:Ӧ��bufferManager��ShaderLib����PipelineState���ϣ���Image��Render������
// ��ʱ����ͨ��pipelineState��imageState����һ��
class PipelineStateManager
{
public:
	PipelineStateManager();
	~PipelineStateManager();

	void Initialize();
	void Uninitialize();

	PipelineState GetNormalPipelineState(RenderPassType renderPassType, MaterialComponent& material, bool forceAlphaTest = false);
	PipelineState GetImagePipelineState(RenderImage::ImageParams params);
	PipelineState GetPipelineStateByID(PipelineStateID stateID);

	void RegisterPipelineState(RenderImage::ImageParams params, PipelineStateDesc desc);
	void RegisterPipelineState(NormalRenderParams params, PipelineStateDesc desc);
	void RegisterPipelineState(PipelineStateID stateID, PipelineStateDesc desc);

private:
	U32 RegisterPipelineState(PipelineStateDesc desc);

	void SetupNormalPipelineStates();
	void SetupImagePipelineStates();
	void SetupPipelineStateIDs();

	std::vector<PipelineState> mPipelineStates;	

	std::map<U32, U32> mImagePipelineStateIndexMap;
	std::map<U32, U32> mNormalPipelineStateIndexMap;
	std::map<U32, U32> mPipelineStateIDMap;
};

}