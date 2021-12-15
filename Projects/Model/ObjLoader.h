#pragma once
#include "Model.h"

namespace ObjLoader
{
	Model LoadFile(std::wstring_view filePath, std::wstring_view fileName);
};

