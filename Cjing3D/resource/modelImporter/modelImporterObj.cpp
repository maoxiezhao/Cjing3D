#include "resource\modelImporter.h"
#include "core\systemContext.hpp"
#include "resource\resourceManager.h"
#include "world\world.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils\tinyObjLoader.h"

#include <filesystem>

namespace Cjing3D
{
	void ImportModelObj(const std::string& fileName, SystemContext& systemContext)
	{
		std::filesystem::path path(fileName);

		auto& resourceManager = systemContext.GetSubSystem<ResourceManager>();
		auto& world = systemContext.GetSubSystem<World>();

		tinyobj::attrib_t objAttrib;
		std::vector <tinyobj::shape_t> objShapes;
		std::vector<tinyobj::material_t> objMaterials;
		std::string obj_errors;

		Logger::Info("Load obj model, model name:" + path.string() + " model path:" + path.parent_path().string());
		std::string parentPath = std::string(path.parent_path().string().c_str()) + "\\";
		bool success = tinyobj::LoadObj(&objAttrib, &objShapes, &objMaterials, &obj_errors, path.string().c_str(), parentPath.c_str(), true);
		if (success == false) {
			Debug::Warning("Failed to open model'" + path.string() + "', " + obj_errors);
			return;
		}

		Scene newScene;


		world.GetMainScene().Merge(newScene);
	}
}