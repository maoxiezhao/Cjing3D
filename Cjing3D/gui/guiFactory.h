#pragma once

#include "gui\guiInclude.h"
#include "gui\widgets.h"

#include <map>
#include <set>

namespace Cjing3D
{
	class GUIStage;

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
	using BaseWidgetCreatorPtr = std::shared_ptr<BaseWidgetCreator>;

	// ͨ�õ�widget��������ֱ�Ӵ���ָ�������ָ��
	template<typename T>
	class WidgetCreator : public BaseWidgetCreator
	{
	public:
		WidgetCreator(GUIStage& guiStage) : BaseWidgetCreator(guiStage, T::GetWidgetTypeStr()) {}

		virtual WidgetPtr Create(const StringID& name)
		{
			return WidgetPtr(new T(mGUIStage, name));
		}
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
			RegisterWidgetType(T::GetWidgetTypeStr(), creatorPtr);
		}

		void RegisterWidgetType(const StringID& typeName, BaseWidgetCreatorPtr creator);

		const std::set<StringID> GetRegisteredWidgetTypes()const {
			return mRegisteredWidgetTypes;
		}

		std::set<StringID>& GetRegisteredWidgetTypes() {
			return mRegisteredWidgetTypes;
		}

	private:
		GUIStage& mGUIStage;

		std::set<StringID> mRegisteredWidgetTypes;
		std::map<StringID, BaseWidgetCreatorPtr> mRegisteredWidgetCreators;
	};
}