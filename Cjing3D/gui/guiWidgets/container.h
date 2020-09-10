#pragma once

#include "gui\guiWidgets\widgets.h"

namespace Cjing3D {
namespace Gui {

	class Container : public Widget
	{
	public:
		Container(GUIStage& stage, const StringID& name = StringID::EMPTY);

		virtual F32x2 CalculateBestSize()const;
		virtual void UpdateLayout();

		void SetSpacing(F32 spacing);
		F32 GetSpacing() { return mSpacing; }
		void SetMargin(const WidgetMargin& margin);
		WidgetMargin GetMargin()const { return mMargin; }
		void SetLayoutDirty();
		std::shared_ptr<Layout> GetLayout() { return mLayout; }
		void SetLayout(const std::shared_ptr<Layout>& layout) { mLayout = layout; }

		virtual void Add(WidgetPtr node);
		virtual void Remove(const StringID& name);
		virtual void Remove(WidgetPtr node);

		void UpdateChildAlignment(AlignmentOrien orien, WidgetPtr child, F32 offset, F32& pos, F32& size);

	protected:
		virtual void UpdateLayoutImpl(const Rect& destRect);
		 
	protected:
		WidgetMargin mMargin;
		F32 mSpacing = 0.0f;
		std::shared_ptr<Layout> mLayout = nullptr;
		bool mNeedLayout = true;
	};

}
}