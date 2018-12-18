#include "transform.h"

namespace Cjing3D
{
	Transform::Transform(SystemContext & context):
		Component(context)
	{
	}

	Transform::~Transform()
	{
	}

	void Transform::Initialize()
	{
	}

	void Transform::Uninitialize()
	{
	}

	void Transform::Update()
	{
	}
	XMMATRIX Transform::GetMatrix()
	{
		return XMMATRIX();
	}
}