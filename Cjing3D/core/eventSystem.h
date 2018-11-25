#pragma once

#include "common\common.h"
#include "helper\stringID.h"
#include "helper\variant.h"

#include <functional>
#include <unordered_map>

namespace Cjing3D
{
/** 事件类型定义 */
enum class EventType {
	EVENT_FRAME_START,
	EVENT_FRAME_TICK,
	EVENT_FRAME_END,
	EVENT_SCENE_RENDER
};

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

	void Subscribe(EventType eventID, subscriber&& func);
	void SubscribeWithMap(EventType eventID, subscriberWithMap&& func);

	void Fire(EventType eventType, const Variant& data = 0);
	void FireWithMap(EventType eventType, const VariantMap& data);

	void Clear();

private:
	EventManager();

	std::unordered_map<EventType, std::vector<subscriber> > mSubscribers;
	std::unordered_map<EventType, std::vector<subscriberWithMap> > mSubscriberWithMaps;
};

/** 全局事件接口 */
#define HANDLE_EVENT_STATIC(function)						[]()					         {return function();}
#define HANDLE_EVENT_VARIANT_STATIC(function)				[](Cjing::Variant var)			 {return function(var);}
#define HANDLE_EVENT_VARIANT_MAP_STATIC(function)			[](Cjing::VariantMap varMap)     {return function(varMap);}
#define HANDLE_EVENT(function) 								[this](Cjing::Variant var)		 {return function();}
#define HANDLE_EVENT_VARIANT(function)						[this](Cjing::Variant var)		 {return function(var);}
#define HANDLE_EVENT_VARIANT_MAP(function)					[this](Cjing::VariantMap varMap) {return function(var);}


#define SUBSCRIBE_TO_EVENT(eventID, function)				Cjing::EventManager::GetInstance().Subscribe(eventID, function);
#define SUBSCRIBE_TO_EVENT_WITH_MAP(eventID, function)      Cjing::EventManager::GetInstance().SubscribeWithMap(eventID, function);

#define FIRE_EVENT(eventID)									Cjing::EventManager::GetInstance().Fire(eventID)	;
#define FIRE_EVENT_VARIANT(eventID, variant)				Cjing::EventManager::GetInstance().Fire(eventID, variant)	;
#define FIRE_EVENT_VARIANT_MAP(eventID, variantMap)			Cjing::EventManager::GetInstance().Fire(eventID, variantMap)	;


}