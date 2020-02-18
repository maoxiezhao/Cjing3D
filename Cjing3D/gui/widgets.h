#pragma once

#include "gui\guiInclude.h"
#include "gui\guiCore\guiDispatcher.h"
#include "utils\treeNode.h"
#include "utils\geometry.h"
#include "helper\enumInfo.h"
#include "scripts\luaContext.h"

#include "utils\tinyxml2\tinyxml2.h"

#include <set>

namespace Cjing3D
{
	class GUIStage;

	struct GUIScriptEventHandlers
	{
		static const StringID OnLoaded;
		static const StringID OnUnloaded;

		static std::set<StringID> registeredScriptEventHandlers;
	};

	LUA_BINDER_REGISTER_CLASS
	class Widget : public TreeNode<Widget>, public Gui::Dispatcher
	{
	public:
		enum WidgetState
		{
			WidgetState_Idle,
			WidgetStage_Focus,
			WidgetStage_Active,
			WidgetStage_Deactive,
		};

		LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
		Widget(GUIStage& stage, const StringID& name = StringID::EMPTY);
		virtual ~Widget();

		virtual void OnLoaded();
		virtual void OnUnloaded();

		virtual void InitProperties(tinyxml2::XMLElement& element);
		virtual void ParseEventHandlers(tinyxml2::XMLElement& element);
		virtual void Update(F32 dt);
		virtual void Render(const F32x2& offset);

		F32x2 TransfromToLocalCoord(const F32x2 point)const;
		bool HitTest(const F32x2& point)const;

		void SetPos(const F32x2 pos);
		F32x2 GetPos()const;
		void SetSize(const F32x2 size);
		F32x2 GetSize()const;
		void SetArea(const Rect& rect);
		Rect GetArea()const { return mArea; }
		void SetEnabled(bool enabled) { mIsEnabled = enabled; }
		bool IsEnabled()const { return mIsEnabled; }

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void SetVisible(bool visible) { mIsVisible = visible;  };
		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		bool IsVisible()const { return mIsVisible; }

		bool IsRoot()const { return  mIsRoot;  }
		void SetIsRoot(bool isRoot) { mIsRoot = isRoot; }

		std::shared_ptr<Widget> FindChildByName(const StringID& name);
		std::shared_ptr<Widget> GetChildWidgetByGlobalCoords(F32x2 pos);
		void RemoveChildByName(const StringID& name);

		LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
		void ClearSelf();

		LuaRef GetScriptHandler();
		void SetScriptHandler(LuaRef handler);
		void AddScriptEventHandler(const StringID& eventName, const std::string& handler);
		void CallScriptEventHandler(const StringID& eventName);
		void CallScriptEventHandlerWithVariants(const StringID& eventName, VariantArray variants);

		WidgetState GetStage()const { return mState; }
		GUIStage& GetGUIStage();

		virtual WidgetType GetSelfWidgetType() const {
			return Widget::GetWidgetType();
		}

		static WidgetType GetWidgetType() { 
			return WidgetType::WidgetType_BaseWidget; 
		}

	protected:
		virtual void RenderImpl(const Rect& destRect);
		
		virtual void OnParentChanged(Widget* old_parent);
		virtual void OnChildAdded(std::shared_ptr<Widget>& node);
		virtual void OnChildRemoved(std::shared_ptr<Widget>& node);

		virtual void RefreshPlacement();

	private:
		GUIStage& mStage;
		WidgetState mState = WidgetState::WidgetState_Idle;
		bool mIsEnabled = true;
		bool mIsVisible = false;
		bool mIsRoot = false;
		bool mIsIgnoreInputEvent = false;

		Rect mArea;

		Color4 mBackColor;
		Color4 mForeColor;
		Color4 mDebugColor;

		LuaRef mScriptHandler;
		std::map<StringID, std::string> mScriptEventHandlers;
	};
	using WidgetPtr = std::shared_ptr<Widget>;

	// register enum infos
	template<>
	struct EnumInfoTraits<WidgetType>
	{
		static const std::string enumName;
		static const EnumInfo<WidgetType>::EnumType enumInfos;
	};
}