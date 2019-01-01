#pragma once

#include "common\common.h"

namespace Cjing3D {

#define GENERATE_ID IDGenerator::GetInstance().GenerateNextGUID()
#define GENERATE_RANDOM_ID IDGenerator::GetInstance().GenerateRandomGUID();

/**
*	\brief 用于生成全局的GUID
*/
class IDGenerator
{
public:
	static IDGenerator& GetInstance();
	U32 GenerateNextGUID()const;
	U32 GenerateRandomGUID()const;

private:
	IDGenerator();
	static U32 mCurGUID;
};

}