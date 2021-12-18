#include "Renderer.h"
#include "ResourceManager.h"
#include "Command.h"
#include "Display.h"

namespace
{
	std::map<std::wstring, std::vector<RenderObject>> s_RenderObjects;
}

void Renderer::Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename)
{
	auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();

	auto model = ResourceManager::GetMesh(modelName);
	auto texture = ResourceManager::GetTexture(texturename);
	renderObject.Create(model.ModelMeshes.at(0), model.ModelMaterials.at(0), texture, 100);
}

gsl::not_null<ID3D12GraphicsCommandList*> Renderer::Begin()
{
	for(auto& model : s_RenderObjects)
	{
		for(auto& mesh : model.second)
		{
			mesh.Initialize();
		}
	}

	return Command::Begin(Display::g_FrameIndex).Get();
}

void Renderer::Draw(
	std::wstring_view assetName,
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 rotation,
	DirectX::XMFLOAT3 scale)
{
	auto& meshVec = s_RenderObjects[assetName.data()];

	for(auto& mesh : meshVec)
	{
		mesh.Draw(position, rotation, scale);
	}
}

void Renderer::SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	for(auto& model : s_RenderObjects)
	{
		for(auto& mesh : model.second)
		{
			mesh.SendCommand(cmdList);
		}
	}
}

void Renderer::End()
{
	Command::End();
}
