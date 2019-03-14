#pragma once

#include "renderer\RHI\rhiResource.h"
#include "resource\resource.h"

namespace Cjing3D
{
	class GraphicsDevice;

	/**
	*	\brief ConstantBuffer��
	*/
	class GPUBuffer : public GPUResource
	{
	public:
		GPUBuffer(GraphicsDevice& device);
		~GPUBuffer();

		// ˢ������
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