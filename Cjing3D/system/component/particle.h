#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {

	class ParticleComponent : public Component
	{
	public:
		ParticleComponent();
		~ParticleComponent();

	public:
		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;


	};

}