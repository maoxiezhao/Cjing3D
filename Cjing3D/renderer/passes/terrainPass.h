#pragma once

#include "renderer\passes\renderPass.h"
#include "renderer\passes\terrainTree.h"

namespace Cjing3D
{
	class TerrainPass : public RenderPass
	{
	public:
		TerrainPass(Renderer& renderer);
		virtual ~TerrainPass();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void UpdatePerFrameData(F32 deltaTime);
		virtual void RefreshRenderData();
		virtual void Render();

		U32 GetElevation()const;
		void SetElevation(U32 elevation);

	private:
		void InitializeShader();
		void InitializeTestData();

		TerrainTreeComponent mTerrainTree;
	};
}