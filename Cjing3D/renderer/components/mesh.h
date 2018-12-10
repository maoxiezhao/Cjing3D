#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiDefinition.h"
#include "resource\resource.h"

namespace Cjing3D
{

struct MeshSubset
{
	StringID mMaterialID;
	U32 mIndexOffset = 0;
	U32 mIndexCount = 0;
}

class Mesh
{
public:
	Mesh(const std::string& name = "");
	~Mesh();

	/**************************************
	*	\brief Mesh Vertex Type
	**************************************/
	struct VertexPosNormalTex
	{
		Point3 pos;
		F32 padding1;
		Normal3 normal;
		F32 padding2;
		UV tex;
		F32x2 padding3;

		VertexPosNormalTex():
			pos(0.0f, 0.0f, 0.0f),
			padding1(0.0f),
			normal(0.0f, 0.0f, 0.0f),
			padding2(0.0f),
			tex(0.0f, 0.0f),
			padding3(0.0f, 0.0f)
		{}
	};

	std::vector<VertexPosNormalTex>& GetVertexPosNormalTex() { return mVertexPosNormalTex; }
	std::vector<U32>& GetIndiecs() { return mIndices; }

private:
	std::vector<VertexPosNormalTex> mVertexPosNormalTex;
	std::vector<U32> mIndices;

	IndexFormat mIndexFormat;
	std::string mName;	

	std::vector<MeshSubset> mMeshSubsets;
};

using MeshPtr = std::shared_ptr<Mesh>;

}