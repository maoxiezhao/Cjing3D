#pragma once

#include "common\common.h"

namespace Cjing3D {

#define GENERATE_ID IDGenerator::GetInstance().GenerateNextGUID()

/**
*	\brief ��������ȫ�ֵ�GUID
*/
class IDGenerator
{
public:
	static IDGenerator& GetInstance();
	U32 GenerateNextGUID()const;

private:
	IDGenerator();
	static U32 mCurGUID;
};

}