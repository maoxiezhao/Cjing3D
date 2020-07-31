#pragma once

#include "definitions\definitions.h"
#include "utils\singleton.h"

namespace CjingGame
{
	// GameContext is a singleton class containing the global game infos
	class GameContext : public Singleton<GameContext>
	{
	public:
		~GameContext();

		void Initialize();
		void Uninitialize();

		// game map
		static const I32 GameMapPartSize = 64;
		static const I32 MapCellSize = 1;
		static const I32 GameMapPartVisibleRange = 128;
		static const I32 GameMapPartRange = 1;
		static const std::string GameMapCurrentParentPath;

	private:
		bool mIsInitialized = false;
	};

}