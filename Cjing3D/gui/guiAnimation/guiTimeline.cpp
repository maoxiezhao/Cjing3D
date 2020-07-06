#include "guiTimeline.h"

namespace Cjing3D
{
namespace Gui
{
	TimeLine::TimeLine()
	{
	}

	TimeLine::~TimeLine()
	{
	}

	void TimeLine::Initialize()
	{
	}

	void TimeLine::FixedUpdate()
	{
		for (const auto& animation : mAnimations)
		{
			if (animation != nullptr) {
				animation->FixedUpdate();
			}
		}
	}

	void TimeLine::Update(F32 delta)
	{
		for (const auto& animation : mAnimations) 
		{
			if (animation != nullptr) {
				animation->Update(delta);
			}
		}

		// remove finished animations
		mAnimations.erase(std::remove_if(
			std::begin(mAnimations), std::end(mAnimations),
			[](const AnimationSharedPtr& p) {
				return p == nullptr || p->IsStoped() || (p->IsRemovedOnFinish() && p->IsFinished());
			}), std::end(mAnimations));

		// process subscribe requests
		for (const auto& request : mSubscribeRequests) {
			if (request != nullptr) {
				mAnimations.push_back(request);
			}
		}
		mSubscribeRequests.clear();
	}

	void TimeLine::PlayAll()
	{
		for (const auto& animation : mAnimations)
		{
			if (animation != nullptr && !animation->IsPlaying()) {
				animation->Play();
			}
		}
		mIsPausing = false;
	}

	void TimeLine::PauseAll()
	{
		for (const auto& animation : mAnimations)
		{
			if (animation != nullptr && !animation->IsPaused()) {
				animation->Pause();
			}
		}
		mIsPausing = true;
	}

	void TimeLine::StopAll()
	{
		for (const auto& animation : mAnimations)
		{
			if (animation != nullptr && !animation->IsStoped()) {
				animation->Stop();
			}
		}
	}

	void TimeLine::Uninitialize()
	{
		mAnimations.clear();
		mSubscribeRequests.clear();
	}

	void TimeLine::AddAnimation(AnimationSharedPtr animation)
	{
		mSubscribeRequests.push_back(std::move(animation));
	}

	void TimeLine::RemoveAnimation(AnimationSharedPtr animation)
	{
		if (animation == nullptr) {
			return;
		}

		auto findFunc = [&](const AnimationSharedPtr p) {
			return p.get() == animation.get();
		};

		auto subIt = std::find_if(std::begin(mSubscribeRequests), std::end(mSubscribeRequests), findFunc);
		if (subIt != std::end(mSubscribeRequests)) {
			mSubscribeRequests.erase(subIt);
			return;
		}

		auto it = std::find_if(std::begin(mAnimations), std::end(mAnimations), findFunc);
		if (it != std::end(mAnimations)) {
			mAnimations.erase(it);
			return;
		}
	}
}
}