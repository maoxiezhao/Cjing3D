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
			// stepģʽ��keyframe����ؼ�֡����
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
			// ��leftKeyFrame��rightKeyFrame��ֵ��ȡtransform
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

	void UpdateSceneAnimationSystem(Scene& scene)
	{
		SystemContext& systemContext = SystemContext::GetSystemContext();
		F32 deltaTime = systemContext.GetDelatTime();

		ECS::ComponentManager<AnimationComponent>& animations = scene.mAnimations;
		ECS::ComponentManager<TransformComponent>& transforms = scene.mTransforms;

		for (size_t index = 0; index < animations.GetCount(); index++)
		{
			AnimationComponent& animation = *animations.GetComponentByIndex(index);
			if (!animation.IsPlaying()) {
				continue;
			}

			F32 currentTime = animation.mCurrentTimer;

			// ����ÿ��animaionChannel����Ӧ��animationSampler�������Ӧ�Ĺؼ�֡ʱ���ֵ
			for (AnimationComponent::AnimationChannel& channel : animation.mChannels)
			{
				auto& sampler = animation.GetAnimationSamplerByChannel(channel);

				// 1. �ҵ���ǰʱ�����������������ؼ�֡
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

				// 2. �ҵ������ؼ�֡��ͨ���ٽ���������Բ�ֵ�ķ�ʽ�����Ӧnode��transform
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
				animation.mCurrentTimer += deltaTime;

				// reset current timer if animation is looped
				if (animation.mCurrentTimer > animation.mTimeEnd && animation.IsLooped()) {
					animation.mCurrentTimer = animation.mTimeStart;
				}
			}
		}
	}

	void UpdateSceneArmatureSystem(Scene& scene)
	{
	}
}