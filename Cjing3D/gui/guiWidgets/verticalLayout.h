#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
	namespace Gui {

		class VerticalLayout : public Widget
		{
		public:
			VerticalLayout(GUIStage& stage, const StringID& name = StringID::EMPTY, F32 width = 1.0f, F32 height = 1.0f);

			virtual bool IsNeedLayout()const;

			void SetMargin(F32 left, F32 top, F32 right, F32 bottom);
			void SetLayoutSpacing(F32 spacing);
			void SetLayoutDirty() { mIsNeedLayout = true; }

			void AddChild(WidgetPtr widget);
			void RemoveChild(WidgetPtr widget);
			void RemoveChild(U32 index);
			U32 GetChildrenCount()const;
			std::vector<WidgetPtr>& GetChildren();

		private:
			virtual void UpdateLayoutImpl(const Rect& destRect);

		private:
			std::vector<WidgetPtr> mChildren;
			bool mIsNeedLayout = false;
			F32 mLayoutSapcing = 12.0f;
			WidgetMargin mMargin;
		};

	}
}