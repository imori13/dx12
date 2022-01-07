#pragma once
#include "Model.h"

namespace AssimpTest {
	bool LoadMesh(Model* model, std::wstring_view fileName);
	bool LoadSkinMesh(SkeletonModel* model, std::wstring_view fileName);
}

