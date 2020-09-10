#include "system\sceneSystem.h"
#include "renderer\renderer.h"

#include <functional>
#include <array>

namespace Cjing3D {
	namespace
	{
		void StepAnimationChannelHandler(
			TransformComponent& transform, AnimationComponent::AnimationSampler& sampler, 
			AnimationComponent::AnimationChannel::TargetPath path, U32 leftKeyFrame, U32 rightKeyFrame)
		{
			// step模式下keyframe向左关键帧对齐
			switch (path)
			{
			case AnimationComponent::AnimationChannel::TargetPath_Scale:
				transform.SetScaleLocal(
					((XMFLOAT3*)sampler.mKeyframeDatas.data())[leftKeyFrame]
				);
				break;
			case AnimationComponent::AnimationChannel::TargetPath_Rotation:
				transform.SetRotationLocal(
					((XMFLOAT4*)sampler.mKeyframeDatas.data())[leftKeyFrame]
				);
				break;
			case AnimationComponent::AnimationChannel::TargetPath_Translation:
				transform.SetTranslationLocal(
					((XMFLOAT3*)sampler.mKeyframeDatas.data())[leftKeyFrame]
				);
				break;
			}
		}

		void LinearAnimationChannelHandler(
			TransformComponent& transform, AnimationComponent::AnimationSampler& sampler,
			AnimationComponent::AnimationChannel::TargetPath path, U32 leftKeyFrame, U32 rightKeyFrame, F32 currentTimer)
		{
			// 对leftKeyFrame和rightKeyFrame插值获取transform
			F32 leftTime = sampler.mKeyframeTimes[leftKeyFrame];
			F32 rightTime = sampler.mKeyframeTimes[rightKeyFrame];
			F32 t = (rightTime - leftTime) > 0.00001f ?
				(currentTimer - leftTime) / (rightTime - leftTime) : 0.0f;
			
			switch (path)
			{
			case AnimationComponent::AnimationChannel::TargetPath_Scale:
			{
				XMFLOAT3* data = ((XMFLOAT3*)sampler.mKeyframeDatas.data());
				XMVECTOR vLeft = XMLoadFloat3(&data[leftKeyFrame]);
				XMVECTOR vRight = XMLoadFloat3(&data[rightKeyFrame]);

				XMFLOAT3 scaleLocal;
				XMStoreFloat3(&scaleLocal, XMVectorLerp(vLeft, vRight, t));
				transform.SetScaleLocal(scaleLocal);
			}	
				break;
			case AnimationComponent::AnimationChannel::TargetPath_Rotation:
			{
				XMFLOAT4* data = ((XMFLOAT4*)sampler.mKeyframeDatas.data());
				XMVECTOR vLeft = XMLoadFloat4(&data[leftKeyFrame]);
				XMVECTOR vRight = XMLoadFloat4(&data[rightKeyFrame]);

				XMFLOAT4 rotationLocal;
				XMStoreFloat4(&rotationLocal, XMQuaternionNormalize(XMQuaternionSlerp(vLeft, vRight, t)));
				transform.SetRotationLocal(rotationLocal);
			}
				break;
			case AnimationComponent::AnimationChannel::TargetPath_Translation:
			{
				XMFLOAT3* data = ((XMFLOAT3*)sampler.mKeyframeDatas.data());
				XMVECTOR vLeft = XMLoadFloat3(&data[leftKeyFrame]);
				XMVECTOR vRight = XMLoadFloat3(&data[rightKeyFrame]);

				XMFLOAT3 translationLocal;
				XMStoreFloat3(&translationLocal, XMVectorLerp(vLeft, vRight, t));
				transform.SetTranslationLocal(translationLocal);
			}
				break;
			}
		}
	}

	void SceneSystem::UpdateSceneAnimationSystem(Scene& scene)
	{
		F32 deltaTime = GetGlobalContext().GetDelatTime();

		ECS::ComponentManager<AnimationComponent>& animations = scene.mAnimations;
		ECS::ComponentManager<TransformComponent>& transforms = scene.mTransforms;

		for (size_t index = 0; index < animations.GetCount(); index++)
		{
			AnimationComponent& animation = *animations.GetComponentByIndex(index);
			if (!animation.IsPlaying()) {
				continue;
			}

			F32 currentTime = animation.mCurrentTimer;

			// 更新每个animaionChannel，对应的animationSampler会包含对应的关键帧时间和值
			for (AnimationComponent::AnimationChannel& channel : animation.mChannels)
			{
				auto& sampler = animation.GetAnimationSamplerByChannel(channel);

				// 1. 找到当前时间所处的左右两个关键帧
				U32 leftKeyIndex = 0;
				U32 rightKeyIndex = 0;

				if (sampler.mKeyframeTimes.back() < currentTime)
				{
					leftKeyIndex = rightKeyIndex = (U32)(sampler.mKeyframeTimes.size() - 1);
				}
				else
				{
					while (sampler.mKeyframeTimes[rightKeyIndex++] < currentTime);
					
					rightKeyIndex = rightKeyIndex - 1;
					leftKeyIndex = (U32)(std::max(0, (int)rightKeyIndex - 1));
				}

				// 2. 找到两个关键帧后，通过临近点或者线性插值的方式处理对应node的transform
				TransformComponent& transform = *transforms.GetComponent(channel.mTargetNode);
				if (sampler.mSamplerMode == AnimationComponent::AnimationSampler::SamplerMode_Step)
				{
					StepAnimationChannelHandler(transform, sampler, channel.mTargetPath, leftKeyIndex, rightKeyIndex);
				}
				else
				{
					LinearAnimationChannelHandler(transform, sampler, channel.mTargetPath, leftKeyIndex, rightKeyIndex, currentTime);
				}
			}

			if (animation.IsPlaying())
			{
				animation.mCurrentTimer += deltaTime * animation.mSpeed;

				// reset current timer if animation is looped
				if (animation.mCurrentTimer > animation.mTimeEnd && animation.IsLooped()) {
					animation.mCurrentTimer = animation.mTimeStart;
				}
			}
		}
	}

	void SceneSystem::UpdateSceneArmatureSystem(Scene& scene)
	{
		ECS::ComponentManager<ArmatureComponent>& armatures = scene.mArmatures;
		ECS::ComponentManager<TransformComponent>& transforms = scene.mTransforms;

		for (size_t index = 0; index < armatures.GetCount(); index++)
		{
			ArmatureComponent& armature = *armatures.GetComponentByIndex(index);

			if (armature.mBonePoses.size() != armature.mSkinningBones.size()) {
				armature.mBonePoses.resize(armature.mSkinningBones.size());
			}

			// bone的transform在world-space下，需要先乘以armature root transform逆矩阵，得到bone的bone-space
			XMMATRIX inverseArmatureM = XMMatrixIdentity();

			auto rootTransform = transforms.GetComponent(armature.mRootBone);
			if (rootTransform != nullptr) {
				inverseArmatureM = XMMatrixInverse(nullptr, XMLoadFloat4x4(&rootTransform->GetWorldTransform()));
			}

			XMFLOAT3 bonePosMin = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
			XMFLOAT3 bonePosMax = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			U32 boneIndex = 0;
			for (ECS::Entity entity : armature.mSkinningBones)
			{
				TransformComponent& boneTransform = *transforms.GetComponent(entity);
				
				XMMATRIX inverseBindM = XMLoadFloat4x4(&armature.mInverseBindMatrices[boneIndex]);
				XMMATRIX boneWorldM = XMLoadFloat4x4(&boneTransform.GetWorldTransform());
				XMMATRIX finalM = inverseBindM * boneWorldM * inverseArmatureM;

				armature.mBonePoses[boneIndex++].Store(finalM);

				XMFLOAT3 bonePos = boneTransform.GetWorldPosition();
				bonePosMin = XMFloat3Min(bonePosMin, bonePos);
				bonePosMax = XMFloat3Max(bonePosMax, bonePos);
			}

			armature.mAABB = AABB(bonePosMin, bonePosMax);

			// 如果buffer未创建，则创建buffer
			if (!armature.IsBoneBufferSetup()) {
				armature.SetupBoneBuffer();
			}
		}
	}
}