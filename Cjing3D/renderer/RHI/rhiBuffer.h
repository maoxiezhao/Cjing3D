#pragma once

#include "renderer\RHI\rhiResource.h"
#include "resource\resource.h"

namespace Cjing3D
{
	class GraphicsDevice;

	/**
	*	\brief ConstantBuffer类
	*/
	class GPUBuffer : public GPUResource
	{
	public:
		GPUBuffer(GraphicsDevice& device);
		~GPUBuffer();

		// 刷新数据
		void Create(U32 size);
		void UpdateData(const void* data, U32 dataSize);

		ID3D11Buffer& GetBuffer() {
			return **((ID3D11Buffer**)&GetGPUResource());
		}

		GPUBufferDesc GetDesc() {
			return mDesc;
		}

		void SetDesc(GPUBufferDesc desc){
			mDesc = desc;
		}
	private:
		GPUBufferDesc mDesc;
	};
}