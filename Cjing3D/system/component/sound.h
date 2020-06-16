#pragma once

#include "system\component\componentInclude.h"
#include "audio\audio.h"

namespace Cjing3D {

	class SoundComponent : public Component
	{
	public:
		SoundComponent();
		~SoundComponent();
		
		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

}