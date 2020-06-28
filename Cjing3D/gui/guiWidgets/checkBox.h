#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
	namespace Gui {

		class CheckBox : public Widget
		{
		public:
			CheckBox(GUIStage& stage, const StringID& name = StringID::EMPTY);
			virtual ~CheckBox();

			bool IsOn()const { return mIsOn; }
			void SetIsOn(bool isOn) { mIsOn = isOn; }

			Signal<void(bool isOn)> OnToggledCallback;

		private:
			void OnMouseClick(const VariantArray& variants);
			virtual void RenderImpl(const Rect& destRect);

			Sprite mBgSprite;
			Sprite mBgOnSprite;
			bool mIsOn = false;
		};

	}
}