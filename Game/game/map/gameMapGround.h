#pragma once

#include "mapInclude.h"

namespace CjingGame
{
	class GameMapGround : public JsonSerializer
	{ 
	public:
		GameMapGround();
		virtual ~GameMapGround();

		void SetTileIndex(U32 index) { mTileIndex = index; }
		U32 GetTileIndex()const { return mTileIndex; }
		void SetLocalPos(const I32x3& localPos) { mLocalPosition = localPos; }
		I32x3 GetLocalPos()const { return mLocalPosition; }

		virtual void Serialize(JsonArchive& archive);
		virtual void Unserialize(JsonArchive& archive)const;

	private:
		U32 mTileIndex = 0;
		I32x3 mLocalPosition = I32x3(0, 0, 0);
	};

	class GameMapDynamicGround : public GameObject
	{
	public:
		GameMapDynamicGround();
		~GameMapDynamicGround();

		U32 GetTileIndex()const { return mTileIndex; }

	private:
		// mTileIndex changed by time
		U32 mTileIndex = 0;	
	};

}