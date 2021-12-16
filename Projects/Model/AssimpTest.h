#pragma once
#include "Model.h"

namespace AssimpTest {
	bool LoadMesh(std::wstring_view fileName, std::vector<ModelMesh>& meshes, std::vector<ModelMaterial>& materials);
}

