#pragma once
#include "Model.h"

namespace ObjLoader
{
	bool LoadFile(Model& model, std::wstring_view filePath, std::wstring_view fileName);
};

