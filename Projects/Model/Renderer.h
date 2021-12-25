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


	void Draw(std::wstring_view assetName, const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection, const gsl::span<Vector3>& positions);
	void Draw(std::wstring_view assetName, const Matrix4x4& world, const Camera& camera, const gsl::span<Vector3>& positions);

	void SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);

	gsl::not_null<ID3D12GraphicsCommandList*> Begin();
	void End(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
}