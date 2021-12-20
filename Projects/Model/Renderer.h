#pragma once
#include "UploadBuffer.h"
#include "RenderObject.h"
#include "Camera.h"

#include <map>

namespace Renderer
{
	void Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename);
	void Load(std::wstring_view assetName, std::wstring_view modelName);

	gsl::not_null<ID3D12GraphicsCommandList*> Begin();

	void Draw(std::wstring_view assetName, const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection);
	void Draw(std::wstring_view assetName, const Matrix4x4& world, const Camera& camera);

	void SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void End(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
}