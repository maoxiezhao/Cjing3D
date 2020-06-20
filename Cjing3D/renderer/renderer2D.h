#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath2D.h"
#include "renderer\rendererUtils.h"
#include "renderer\sprite.h"

namespace Cjing3D
{
	class SpritieRenderBatchQueue;

	class Renderer2D
	{
	public:
		Renderer2D();
		~Renderer2D();

		void Initialize();
		void Uninitialize();
		void Render();

		void AddSprite(Sprite* sprite);
		void RemoveSprite(Sprite* sprite);
		
	private:
		std::vector<Sprite*> mSprites;
	};

}