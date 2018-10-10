#include "idGenerator.h"

namespace Cjing3D {
	U64 IDGenerator::mCurGUID = 0;

	IDGenerator & IDGenerator::GetInstance()
	{
		static IDGenerator instance;
		return instance;
	}

	U64 IDGenerator::GenerateNextGUID() const
	{
		return mCurGUID++;
	}

	IDGenerator::IDGenerator()
	{
	}
}