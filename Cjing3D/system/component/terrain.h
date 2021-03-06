#pragma once

#include "system\component\componentInclude.h"
#include "renderer\passes\terrain\terrainTree.h"

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
		TextureResourcePtr GetHeightMap();
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
		TextureResourcePtr mHeightMap = nullptr;

		std::string mWeightTextureName;
		std::string mDetailTextureName1;
		std::string mDetailTextureName2;
		std::string mDetailTextureName3;
		TerrainMaterial mMaterial;
	};

}

