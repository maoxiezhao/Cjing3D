#include "idGenerator.h"

namespace Cjing3D {
	U32 IDGenerator::mCurGUID = 0;

	IDGenerator & IDGenerator::GetInstance()
	{
		static IDGenerator instance;
		return instance;
	}

	U32 IDGenerator::GenerateNextGUID() const
	{
		return mCurGUID++;
	}

	IDGenerator::IDGenerator()
	{
	}
}