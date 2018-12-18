#include "mesh.h"

namespace Cjing3D
{

Mesh::Mesh(const std::string& name):
	Resource(Resource_Mesh),
	mName(name)
{
}

Mesh::~Mesh()
{
}

void Mesh::AddMeshSubset(MeshSubset subset)
{
	mMeshSubsets.push_back(subset);
}

const std::vector<Mesh::MeshSubset>& Mesh::GetMeshSubsets() const
{
	return mMeshSubsets;
}

}