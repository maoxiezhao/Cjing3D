#include "system\sceneSystem.h"

namespace Cjing3D
{
	void UpdateSceneSoundSystem(Scene& scene)
	{
		auto& audioManager = GlobalGetSubSystem<Audio::AudioManager>();
		CameraComponent& camera = Renderer::GetCamera();

		Audio::SoundInstance3D instance3D;
		instance3D.listenerPos   = XMConvert(camera.GetCameraPos());
		instance3D.listenerUp    = XMConvert(camera.GetCameraUP());
		instance3D.listenerFront = XMConvert(camera.GetCameraFront());

		for (size_t i = 0; i < scene.mSounds.GetCount(); i++)
		{
			SoundComponent& sound = *scene.mSounds[i];
			Entity entity = scene.mSounds.GetEntityByIndex(i);

			TransformComponent* transform = scene.mTransforms.GetComponent(entity);
			if (transform != nullptr)
			{
				instance3D.emitterPos = transform->GetWorldPosition();
				audioManager.Update3D(sound.mSoundInstance, instance3D);
			}

			if (sound.IsDirty())
			{
				sound.mIsDirty = false;

				if (sound.IsPlaying())
				{
					audioManager.Play(sound.mSoundInstance);
				}
				else
				{
					audioManager.Stop(sound.mSoundInstance);
				}
				audioManager.SetVolume(sound.mSoundInstance, sound.mVolume);
			}
			
		}
	}
}