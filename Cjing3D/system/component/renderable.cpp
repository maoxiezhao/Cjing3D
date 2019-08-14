#include "renderable.h"

namespace Cjing3D
{
	Renderable::Renderable(SystemContext & systemContext):
		Component(systemContext)
	{
	}

	Renderable::~Renderable()
	{
	}

	void Renderable::Initialize()
	{
	}

	void Renderable::Uninitialize()
	{
	}

	void Renderable::Update()
	{
	}

	AABB Renderable::GetBoundingBox()
	{
		return AABB();
	}
}