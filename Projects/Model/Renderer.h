#pragma once
#include "Matrix4x4.h"
#include "Camera.h"

namespace Renderer
{
	void Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename, int32_t objectCount = 1);
	void Load(std::wstring_view assetName, std::wstring_view modelName, int32_t objectCount = 1);

	void Draw(std::wstring_view assetName, Matrix4x4 matrixData);
	//void Draw(std::wstring_view assetName, gsl::span<Matrix4x4> matrixData);

	gsl::not_null<ID3D12GraphicsCommandList*> Begin(const Camera& camera);
	void End(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
}