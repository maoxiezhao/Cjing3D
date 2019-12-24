#include "widgets.h"

namespace Cjing3D
{
	Widget::Widget(GUIStage& stage, const StringID& name) :
		mStage(stage),
		mName(name)
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

	GUIStage& Widget::GetGUIStage()
	{
		return mStage;
	}
}