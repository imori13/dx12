#pragma once
#include "UploadBuffer.h"
#include "RenderObject.h"

#include <map>

namespace Renderer
{
	void Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename);
	void Load(std::wstring_view assetName, std::wstring_view modelName);

	gsl::not_null<ID3D12GraphicsCommandList*> Begin();

	void Draw(
	std::wstring_view assetName,
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 rotation,
	DirectX::XMFLOAT3 scale);

	void SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void End();
}