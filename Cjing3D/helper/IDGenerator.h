#pragma once

#include "common\common.h"

namespace Cjing3D {

	/**
	*	\brief 用于生成全局的GUID
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