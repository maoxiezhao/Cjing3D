#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath2D.h"
#include "renderer\rendererUtils.h"
#include "renderer\sprite.h"

namespace Cjing3D
{
	class SpritieRenderBatchQueue;

	namespace Renderer2D
	{
		void Initialize();
		void Uninitialize();

		void RenderSprites();
		void AddSprite(Sprite* sprite);
		void RemoveSprite(Sprite* sprite);
		
		void RequestRenderSprite(Sprite* sprite);
	};

}