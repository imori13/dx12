#pragma once
#include "UploadBuffer.h"
#include "RenderObject.h"
#include "Camera.h"

#include <map>

namespace Renderer
{
	void Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename, uint32_t objectCount = 1);
	void Load(std::wstring_view assetName, std::wstring_view modelName, uint32_t objectCount = 1);

	gsl::not_null<ID3D12GraphicsCommandList*> Begin();

	void Draw(std::wstring_view assetName, const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection,uint32_t index);
	void Draw(std::wstring_view assetName, const Matrix4x4& world, const Camera& camera, uint32_t index);

	void SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void End(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
}