#include "camera.h"

namespace Cjing3D
{
	Camera::Camera(SystemContext& systemContext):
		Component(systemContext),
		mIsDirty(false)
	{
		mNearPlane = 0.001f;
		mFarPlane = 800.0f;
		mFov = XM_PI / 3.0f;

		mEye = { 0.0f, 0.0f, 0.0f };
		mAt =  { 0.0f, 0.0f, 1.0f };
		mUp =  { 0.0f, 1.0f, 0.0f };

		mProjectionType = CameraProjectionType_Perspective;
	}

	Camera::~Camera()
	{
	}

	void Camera::Initialize()
	{
	}

	void Camera::Uninitialize()
	{
	}

	void Camera::Update()
	{
	}

	void Camera::SetupPerspective(F32 width, F32 height, F32 nearPlane, F32 farPlane, F32 fov)
	{
	}

	void Camera::ComputeViewMatrix()
	{
	}

	void Camera::ComputeProjection()
	{
	}
}