#include "widgets.h"
#include "gui\guiStage.h"
#include "renderer\paths\renderImage.h"
#include "renderer\textureHelper.h"
#include "renderer\renderer.h"
#include "core\systemContext.hpp"
#include "helper\random.h"

namespace Cjing3D
{
	const std::string EnumInfoTraits<WidgetType>::enumName = "widget_type";
	const EnumInfo<WidgetType>::EnumType EnumInfoTraits<WidgetType>::enumInfos =
	{
		{WidgetType::WidgetType_BaseWidget, "BaseWidget"},
	};

	Widget::Widget(GUIStage& stage, const StringID& name) :
		TreeNode<Widget>(name),
		mStage(stage)
	{
#ifdef CJING_GUI_DEBUG
		mDebugColor.SetA(255);
		mDebugColor.SetR(Random::GetRandomInt(255));
		mDebugColor.SetG(Random::GetRandomInt(255));
		mDebugColor.SetB(Random::GetRandomInt(255));
#endif
	}

	Widget::~Widget()
	{
	}

	void Widget::Update(F32 dt)
	{
		if (!IsVisible()) {
			return;
		}
	}

	void Widget::Render(const F32x2& offset)
	{
		if (!IsVisible() || mArea.GetSize()[0] <= 0.0f) {
			return;
		}

		Rect destRect(mArea);
		destRect.Offset(offset);

		RenderImpl(destRect);
	
		F32x2 childOffset = destRect.GetPos();
		for (auto& child : mChildren){
			child->Render(offset);
		}
	}

	F32x2 Widget::TransfromToLocalCoord(const F32x2 point) const
	{
		F32x2 result = point;
		if (mParent != nullptr)
		{
			result -= mParent->GetArea().GetPos();
			result = mParent->TransfromToLocalCoord(result);
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

	void Widget::RenderImpl(const Rect& destRect)
	{
		SystemContext& systemContext = SystemContext::GetSystemContext();
		Renderer& renderer = systemContext.GetSubSystem<Renderer>();

#ifdef CJING_GUI_DEBUG
		F32x2 pos = destRect.GetPos();
		F32x2 size = destRect.GetSize();

		RenderImage::ImageParams params(pos[0], pos[1], size[0], size[1], mDebugColor.ToFloat4());
		RenderImage::Render(*TextureHelper::GetWhite(), params, renderer);
#endif

	}

	void Widget::onParentChanged(Widget* old_parent)
	{
	}

	void Widget::onChildAdded(std::shared_ptr<Widget>& node)
	{
	}

	void Widget::onChildRemoved(std::shared_ptr<Widget>& node)
	{
	}
}