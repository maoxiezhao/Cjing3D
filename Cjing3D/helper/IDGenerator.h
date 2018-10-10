#pragma once

#include "common\common.h"

namespace Cjing3D {

	/**
	*	\brief ��������ȫ�ֵ�GUID
	*/
	class IDGenerator
	{
	public:
		static IDGenerator& GetInstance();
		U64 GenerateNextGUID()const;

	private:
		IDGenerator();

		static U64 mCurGUID;
	};
}