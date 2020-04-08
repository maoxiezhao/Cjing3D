#pragma once

#include "system\component\componentInclude.h"

namespace Cjing3D {
	  
	class ArmatureComponent : public Component
	{
	public:
		ArmatureComponent();
		~ArmatureComponent();

		std::vector<ECS::Entity> mSkiningBones;
		std::vector<XMFLOAT4X4> mInverseBindMatrices;

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

	class Animation : public Component
	{
	public:
		Animation();
		~Animation();

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;
	};

}

