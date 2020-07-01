#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
	namespace Gui {

		class NinePatchImage : public Widget
		{
		public:
			NinePatchImage(GUIStage& stage, const StringID& name = StringID::EMPTY, const std::string& path = "");

		protected:
			virtual void RenderImpl(const Rect& destRect);

		
		};

	}
}