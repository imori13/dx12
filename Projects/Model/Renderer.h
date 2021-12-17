#pragma once
#include "UploadBuffer.h"
#include "RenderObject.h"

#include <map>

namespace Renderer
{
	void Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename);
	void Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, std::wstring_view assetName);
}