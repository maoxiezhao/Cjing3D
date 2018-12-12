#pragma once

#include "renderer\RHI\rhiResource.h"
#include "resource\resource.h"

namespace Cjing3D
{
	class GraphicsDevice;

	/**
	*	\brief ConstantBuffer类
	*/
	class GPUBuffer
	{
	public:
		GPUBuffer(GraphicsDevice& device) :mDevice(device) {}
		~GPUBuffer() = default;

		// 刷新数据
		void Create(U32 size);
		void UpdateData(const void* data, U32 dataSize);

		ID3D11Buffer& GetBuffer() {
			return *mBuffer.Get();
		}

		ComPtr<ID3D11Buffer>& GetBufferPtr() {
			return mBuffer;
		}

		GPUBufferDesc GetDesc() {
			return mDesc;
		}

		void SetDesc(GPUBufferDesc desc){
			mDesc = desc;
		}
	private:
		GPUBufferDesc mDesc;
		GraphicsDevice & mDevice;
		ComPtr<ID3D11Buffer> mBuffer;
	};
}