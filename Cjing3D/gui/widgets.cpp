#include "widgets.h"
#include "gui\guiStage.h"

namespace Cjing3D
{
	Widget::Widget(GUIStage& stage, const StringID& name) :
		TreeNode<Widget>(name),
		mStage(stage)
	{
	}

	Widget::~Widget()
	{
	}

	void Widget::Update(F32 dt)
	{
	}

	void Widget::Render()
	{
	}

	void Widget::RenderChilds()
	{
	}

	F32x2 Widget::TransfromToLocalCoord(const F32x2 point) const
	{
		F32x2 result = point;
		if (mParent != nullptr)
		{

		}

		return result;
	}

	// simple version. just check rect.
	bool Widget::HistTest(const F32x2& point) const
	{
		if (mIsVisible == false || mIsIgnoreInputEvent) {
			return false;
		}

		return mArea.Intersects(TransfromToLocalCoord(point));
	}

	void Widget::SetArea(const Rect& rect)
	{
		mArea = rect;
	}

	GUIStage& Widget::GetGUIStage()
	{
		return mStage;
	}

	void Widget::onParentChanged(Widget* old_parent)
	{
	}

	void Widget::onChildAdded(WidgetPtr& node)
	{
	}

	void Widget::onChildRemoved(WidgetPtr& node)
	{
	}
}