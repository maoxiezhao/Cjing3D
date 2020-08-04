#pragma once

#include "mapInclude.h"

namespace CjingGame
{
	class GameMapObject : public JsonSerializer
	{
	public:
		GameMapObject();
		virtual ~GameMapObject();

		virtual void Serialize(JsonArchive& archive);
		virtual void Unserialize(JsonArchive& archive)const;
	};
}