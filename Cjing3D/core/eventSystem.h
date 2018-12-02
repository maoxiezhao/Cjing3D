#pragma once

#include "common\common.h"
#include "helper\stringID.h"
#include "helper\variant.h"
#include "core\eventDefine.h"

#include <functional>
#include <unordered_map>

namespace Cjing3D
{

// 全局单例事件系统
class EventManager
{
public:
	using subscriber = std::function<void(Variant)>;
	using subscriberWithMap = std::function<void(VariantMap)>;

	static EventManager& GetInstance()
	{
		static EventManager instance;
		return instance;
	}

	void Register(EventType eventID, subscriber&& func);
	void RegisterWithMap(EventType eventID, subscriberWithMap&& func);

	void Fire(EventType eventType, const Variant& data = 0);
	void FireWithMap(EventType eventType, const VariantMap& data);

	void Clear();

private:
	EventManager();

	std::unordered_map<EventType, std::vector<subscriber> > mSubscribers;
	std::unordered_map<EventType, std::vector<subscriberWithMap> > mSubscriberWithMaps;
};

/** 全局事件接口 */
#define HANDLE_EVENT_STATIC(function)						[]()					           {return function();}
#define HANDLE_EVENT_VARIANT_STATIC(function)				[](Cjing3D::Variant var)		   {return function(var);}
#define HANDLE_EVENT_VARIANT_MAP_STATIC(function)			[](Cjing3D::VariantMap varMap)     {return function(varMap);}
#define HANDLE_EVENT(function) 								[this](Cjing3D::Variant var)	   {return function();}
#define HANDLE_EVENT_VARIANT(function)						[this](Cjing3D::Variant var)	   {return function(var);}
#define HANDLE_EVENT_VARIANT_MAP(function)					[this](Cjing3D::VariantMap varMap) {return function(varMap);}


#define REGISTER_TO_EVENT(eventID, function)				Cjing3D::EventManager::GetInstance().Register(eventID, function);
#define REGISTER_TO_EVENT_WITH_MAP(eventID, function)       Cjing3D::EventManager::GetInstance().RegisterWithMap(eventID, function);

#define FIRE_EVENT(eventID)									Cjing3D::EventManager::GetInstance().Fire(eventID)	;
#define FIRE_EVENT_VARIANT(eventID, variant)				Cjing3D::EventManager::GetInstance().Fire(eventID, variant)	;
#define FIRE_EVENT_VARIANT_MAP(eventID, variantMap)			Cjing3D::EventManager::GetInstance().Fire(eventID, variantMap)	;


}