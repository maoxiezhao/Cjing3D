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

	private:
		bool mIsInitialized = false;
	};

}