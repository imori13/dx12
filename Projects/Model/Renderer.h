#pragma once
#include "UploadBuffer.h"
#include "RenderObject.h"
#include "Camera.h"
#include "Matrix4x4.h"

#include <map>

namespace Renderer
{
	void Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename, int32_t objectCount = 1);
	void Load(std::wstring_view assetName, std::wstring_view modelName, int32_t objectCount = 1);

	void Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, std::wstring_view assetName, gsl::span<Matrix4x4> matrixData);

	gsl::not_null<ID3D12GraphicsCommandList*> Begin(Matrix4x4 view, Matrix4x4 proj);
	void End(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
}