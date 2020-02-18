#pragma once

#include "gui\guiInclude.h"
#include "gui\widgets.h"

#include <map>
#include <set>

namespace Cjing3D
{
	class GUIStage;

	inline StringID ConvertToWidgetTypeStrByWidgetType(WidgetType widgetType) {
		const std::string widgetTypeStr = EnumToString(widgetType, "");
		if (widgetTypeStr.empty()) {
			return StringID::EMPTY;
		}

		return StringID(widgetTypeStr);
	}

	class BaseWidgetCreator
	{
	public:
		BaseWidgetCreator(GUIStage& guiStage, const StringID& typeName) : 
			mGUIStage(guiStage), mType(typeName) {};
		virtual ~BaseWidgetCreator() {}

		virtual WidgetPtr Create(const StringID& name) = 0;

	protected:
		StringID mType;
		GUIStage& mGUIStage;
	};
	using WidgetCreatorPtr = std::shared_ptr<BaseWidgetCreator>;

	// ͨ�õ�widget��������ֱ�Ӵ���ָ�������ָ��
	template<typename T>
	class WidgetCreator : public BaseWidgetCreator
	{
	public:
		WidgetCreator(GUIStage& guiStage) : BaseWidgetCreator(guiStage, ConvertToWidgetTypeStrByWidgetType(T::GetWidgetType()) ) {}

		virtual WidgetPtr Create(const StringID& name)
		{
			return WidgetPtr(new T(mGUIStage, name));
		}
	};

	class DefaultWidgetCreator : public BaseWidgetCreator
	{
	public:
		DefaultWidgetCreator(GUIStage& guiStage) : BaseWidgetCreator(guiStage, ConvertToWidgetTypeStrByWidgetType(Widget::GetWidgetType())) {}
		virtual WidgetPtr Create(const StringID& name);
	};

	// widget�Ĺ����࣬�����Ӧwidget�Ĺ�������ͬʱ֧��ע��ͨ��widget�Ĺ��������Զ���Ĺ�����
	class GUIFactory
	{
	public:
		GUIFactory(GUIStage& guiStage);
		~GUIFactory();

		template<typename T>
		void RegisterWidgetType()
		{
			auto creatorPtr = std::make_shared<WidgetCreator<T>>(mGUIStage);
			StringID typeStringID = ConvertToWidgetTypeStrByWidgetType(T::GetWidgetType());
			RegisterWidgetType(typeStringID, creatorPtr);
		}

		void RegisterWidgetType(const StringID& typeName, WidgetCreatorPtr creator);

		const std::set<StringID> GetRegisteredWidgetTypes()const {
			return mRegisteredWidgetTypes;
		}

		std::set<StringID>& GetRegisteredWidgetTypes() {
			return mRegisteredWidgetTypes;
		}

		WidgetPtr CreateWidget(const StringID& type, const StringID& name);

	private:
		GUIStage& mGUIStage;

		std::set<StringID> mRegisteredWidgetTypes;
		std::map<StringID, WidgetCreatorPtr> mRegisteredWidgetCreators;
	};
}