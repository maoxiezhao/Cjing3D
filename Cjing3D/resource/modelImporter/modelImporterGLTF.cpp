#include "resource\modelImporter.h"
#include "core\systemContext.hpp"
#include "resource\resourceManager.h"
#include "system\sceneSystem.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "utils\tinygltf\tiny_gltf.h"

#include <filesystem>

namespace Cjing3D {
namespace ModelImporter {

	void ImportModelGLTF(const std::string& fileName, SystemContext& systemContext)
	{
	}
}
}