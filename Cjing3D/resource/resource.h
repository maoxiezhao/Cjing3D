#pragma once

#include "common\common.h"
#include "helper\stringID.h"

namespace Cjing3D
{
	class ResourceManager;

	enum Resource_Type
	{
		Resource_Unknown,
		Resrouce_VertexShader,
		Resrouce_PixelShader,
		Resource_Texture,
		Resource_Mesh,
		Resource_Model
	};

	/**
	*	\brief 资源类基类
	*/
	class Resource : public std::enable_shared_from_this<Resource>
	{
	public:
		Resource(Resource_Type type);
		Resource(const Resource& rhs) = delete;
		Resource(Resource&& rhs) = default;

		Resource& operator= (const Resource& other) = delete;
		Resource& operator= (Resource&& other) = default;

		virtual ~Resource() {};

		Resource_Type GetResourceType()const { return mType; }
		U32 GetGUID()const { return mGUID; }
		
		StringID GetResourceName()const { return mName; }
		void SetResourceName(const std::string& name) { mName = StringID(name); }

		virtual bool SaveToFile(const std::string& filePath) { return false; };
		virtual bool LoadFromFile(const std::string& filePath) { return false; };

		template <typename T>
		static Resource_Type DeduceResourceType();

	private:
		U32 mGUID;
		StringID mName;
		Resource_Type mType;
	};

	using ResourcePtr = std::shared_ptr<Resource>;
}