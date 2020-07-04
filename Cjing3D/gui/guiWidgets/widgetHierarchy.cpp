#include "widgetHierarchy.h"
#include "gui\guiStage.h"

namespace Cjing3D
{
namespace Gui
{
	WidgetHierarchy::WidgetHierarchy(GUIStage& guiStage) :
		mGUIStage(guiStage)
	{
	}

	WidgetHierarchy::~WidgetHierarchy()
	{
	}

	void WidgetHierarchy::Initialize(U32x2 size)
	{

	}

	void WidgetHierarchy::Uninitialize()
	{
		ClearWidgets();
	}

	void WidgetHierarchy::Update(F32 deltaTime)
	{
		for (auto& widget : mWidgets) {
			if (widget != nullptr) {
				widget->Update(deltaTime);
			}
		}
	}

	void WidgetHierarchy::FixedUpdate()
	{
		RefreshWidgets();

		// fixed update
		for (auto& widget : mWidgets) {
			if (widget != nullptr) {
				widget->FixedUpdate();
			}
		}

		UpdateLayout();
	}

	void WidgetHierarchy::Render()
	{
		F32x2 offset = { 0.0f, 0.0f };
		for (auto it = mWidgets.rbegin(); it != mWidgets.rend(); it++)
		{
			auto widget = *it;
			if (widget != nullptr) {
				widget->Render(offset);
			}
		}
	}

	void WidgetHierarchy::HandleInputEvents(const GUIInputEvent& inputEvent)
	{
		mEventDistributor.HandleInpueEvent(inputEvent, mWidgets);
	}

	void WidgetHierarchy::RefreshWidgets()
	{
		if (!mSubscribeRequests.empty()) 
		{
			mIsHierarchySortDirty = true;

			if (mWidgets.empty()) 
			{
				std::swap(mWidgets, mSubscribeRequests);
			}
			else 
			{
				mWidgets.reserve(mWidgets.size() + mSubscribeRequests.size());
				mWidgets.insert(std::end(mWidgets), std::begin(mSubscribeRequests), std::end(mSubscribeRequests));
				mSubscribeRequests.clear();
			}
		}

		// remove null widgets
		mWidgets.erase( std::remove_if(std::begin(mWidgets), std::end(mWidgets),
			[](const WidgetPtr& p) { return p == nullptr; }), std::end(mWidgets));
		
		if (mIsHierarchySortDirty)
		{
			mIsHierarchySortDirty = false;

			std::stable_sort(std::begin(mWidgets), std::end(mWidgets),
				[&](const std::shared_ptr<Widget>& a, const std::shared_ptr<Widget>& b) -> bool 
				{
					const auto x = a->GetHierarchySortOrder();
					const auto y = b->GetHierarchySortOrder();
					if (x != y) return x < y;
					return a->GetOrderValue() < b->GetOrderValue();
				});
		}
	}

	void WidgetHierarchy::UpdateLayout()
	{
		for (auto it = mSubscribeRequests.begin(); it != mSubscribeRequests.end(); it++)
		{
			auto widget = *it;
			if (widget != nullptr) {
				F32x2 bestSize = widget->GetBestSize();
				widget->SetSize(bestSize);
				widget->UpdateLayout();
			}
		}

		for (auto it = mWidgets.begin(); it != mWidgets.end(); it++)
		{
			auto widget = *it;
			if (widget != nullptr) {
				F32x2 bestSize = widget->GetBestSize();
				widget->SetSize(bestSize);
				widget->UpdateLayout();
			}
		}
	}

	void WidgetHierarchy::ClearWidgets()
	{
		for (auto it = mSubscribeRequests.begin(); it != mSubscribeRequests.end(); it++)
		{
			if ((*it) != nullptr) {
				(*it)->Clear();
			}
		}
		mSubscribeRequests.clear();

		for (auto it = mWidgets.begin(); it != mWidgets.end(); it++)
		{
			if ((*it) != nullptr) {
				(*it)->Clear();
			}
		}
		mWidgets.clear();
	}

	void WidgetHierarchy::CaptureFocusWidget(bool captured)
	{
		mEventDistributor.SetMouseCaptured(captured);
	}

	WidgetPtr WidgetHierarchy::GetCurrentMouseFocusdWidget()
	{
		return mEventDistributor.GetMouseFocusWidget();
	}

	WidgetPtr WidgetHierarchy::GetCurrentDragWidget()
	{
		return mEventDistributor.GetMouseDragWidget();
	}

	WidgetPtr WidgetHierarchy::GetCurrentFocusdWidget()
	{
		return mEventDistributor.GetCurrentFocusdWidget();
	}

	void WidgetHierarchy::SetCurrentFocusedWidget(WidgetPtr widget)
	{
		mEventDistributor.SetCurrentFocusedWidget(widget);
	}

	void WidgetHierarchy::SetWidgetHierarchySort(Widget& widget, HierarchySortOrder order)
	{
		if (widget.GetHierarchySortOrder() != order)
		{
			widget.SetHierarchySortOrder(order);
			mIsHierarchySortDirty = true;
		}
	}

	Connection WidgetHierarchy::ConnectFocusedWidgetChanged(std::function<void(WidgetPtr oldWidget, WidgetPtr newWidge)> func)
	{
		return mEventDistributor.OnFocusedChanged.Connect(func);
	}

	void WidgetHierarchy::AddWidget(WidgetPtr widget)
	{
		if (widget == nullptr) {
			return;
		}

		mSubscribeRequests.push_back(widget);
	}

	void WidgetHierarchy::RemoveWidget(WidgetPtr widget)
	{
		if (widget == nullptr) {
			return;
		}

		auto findFunc = [&](const WidgetPtr p) { 
			return p.get() == widget.get(); 
		};

		auto it = std::find_if(std::begin(mWidgets), std::end(mWidgets), findFunc);
		if (it != std::end(mWidgets)) {
			it->reset();
			return;
		}

		it = std::find_if(std::begin(mSubscribeRequests), std::end(mSubscribeRequests), findFunc);
		if (it != std::end(mSubscribeRequests)) {
			it->reset();
			return;
		}
	}
}
}