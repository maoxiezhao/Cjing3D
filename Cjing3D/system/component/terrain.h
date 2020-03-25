#pragma once

#include "system\component\componentInclude.h"
#include "renderer\passes\terrainTree.h"

namespace Cjing3D {

	LUA_BINDER_REGISTER_CLASS
	class TerrainComponent : public Component
	{
	public:
		TerrainComponent();
		~TerrainComponent();

		void SetElevation(U32 elevation);
		U32 GetElevation()const;
		void SetTerrainSize(U32 width, U32 height);
		U32x2 GetTerrainSize()const { return { mTerrainWidth, mTerrainHeight }; }
		void LoadHeightMap(const std::string& name);
		Texture2DPtr GetHeightMap();
		void LoadDetailMap(const std::string& weightMapName, const std::string& detailName1, const std::string& detailName2, const std::string& detailName3);
		TerrainMaterial& GetTextureMaterial();

		bool IsTerrainDirty()const { return mIsTerrainDirty;}
		void SetTerrainDirty(bool isDirty) { mIsTerrainDirty = isDirty; }

		virtual void Serialize(Archive& archive, U32 seed = 0);
		virtual void Unserialize(Archive& archive)const;

	private:
		bool mIsTerrainDirty = true;

		U32 mTerrainWidth = 512;
		U32 mTerrainHeight = 512;
		U32 mTerrainElevation = 0;
		std::string mHeightMapName;
		Texture2DPtr mHeightMap = nullptr;

		std::string mWeightTexture;
		std::string mDetailTexture1;
		std::string mDetailTexture2;
		std::string mDetailTexture3;
		TerrainMaterial mMaterial;
	};

}

