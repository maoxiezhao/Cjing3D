#pragma once

#include "common\common.h"
#include "utils\string\utf8String.h"
#include "utils\geometry.h"

namespace Cjing3D {

	class GameWindow
	{
	public:
		GameWindow() = default;
		GameWindow(const GameWindow&) = delete;
		GameWindow& operator=(const GameWindow&) = delete;
		virtual ~GameWindow() = default;

		virtual void* GetWindowHandle() const = 0;
		virtual bool IsWindowActive() const = 0;
		virtual bool IsFullScreen()const = 0;
		virtual UTF8String GetWindowTitle()const = 0;
		virtual void SetWindowTitle(const UTF8String& title) = 0;
		virtual bool IsMouseCursorVisible()const = 0;
		virtual void SetMouseCursorVisible(bool visible) = 0;
		virtual RectInt GetClientBounds()const = 0;
		virtual void SetClientbounds(const RectInt& rect) = 0;
		virtual I32 GetDPI()const = 0;
	};
}