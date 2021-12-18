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
	auto model = ResourceManager::GetMesh(modelName);

	auto texture = ResourceManager::GetTexture(texturename);

	for(const auto& mesh : model.ModelMeshes)
	{
		auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();
		renderObject.Create(mesh, model.ModelMaterials.at(mesh.MaterialId), texture, 100);
	}
}

void Renderer::Load(std::wstring_view assetName, std::wstring_view modelName)
{
	auto model = ResourceManager::GetMesh(modelName);

	for(const auto& mesh : model.ModelMeshes)
	{
		const auto& material = model.ModelMaterials.at(mesh.MaterialId);

		auto textureName = (!material.DiffuseMap.empty())?(material.DiffuseMap):(L"body_tex.tga");
		auto texture = ResourceManager::GetTexture(textureName);

		auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();
		renderObject.Create(mesh, material, texture, 100);
	}
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
