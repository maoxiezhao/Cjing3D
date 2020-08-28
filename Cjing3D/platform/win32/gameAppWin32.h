#ifdef _WIN32
#pragma once

#include "common\common.h"
#include "platform\win32\includeWin32.h"
#include "utils\string\utf8String.h"
#include "utils\signal\eventQueue.h"

#include <functional>

namespace Cjing3D
{
	class GameComponent;
	class Engine;
}
namespace Cjing3D::Win32
{
	class GameAppWin32
	{
	public:
		GameAppWin32();
		~GameAppWin32() = default;

		void SetInstance(HINSTANCE hInstance);
		void SetAssetPath(const std::string& path, const std::string& name);
		void SetTitleName(const UTF8String& string);
		void SetScreenSize(const I32x2& screenSize);

		using CreateGameFunc = std::function<std::unique_ptr<GameComponent>(const std::shared_ptr<Engine>)>;
		void Run(const CreateGameFunc& createGame);

	private:
		HINSTANCE   mHinstance;
		std::string mAssetName = "";
		std::string mAssetPath = "Assets";
		UTF8String  mTitleName;
		I32x2       mScreenSize = I32x2(0, 0);
	};
}

#endif
