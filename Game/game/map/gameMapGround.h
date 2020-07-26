#pragma once

#include "game\gameObject.h"

namespace CjingGame
{
	class GameMapGround : public GameObject
	{
	public:
		GameMapGround();
		virtual ~GameMapGround();

		void SetTileIndex(U32 index) { mTileIndex = index; }
		U32 GetTileIndex()const { return mTileIndex; }

	private:
		U32 mTileIndex = 0;
	};
}