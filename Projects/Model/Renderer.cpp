#include "Renderer.h"
#include "ResourceManager.h"

namespace
{
	std::map<std::wstring, std::vector<RenderObject>> s_RenderObjects;
}

void Renderer::Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename)
{
	auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();

	auto model = ResourceManager::GetMesh(modelName);
	auto texture = ResourceManager::GetTexture(texturename);
	renderObject.Create(model.ModelMeshes.at(0), model.ModelMaterials.at(0), texture, 1);
}

void Renderer::Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, std::wstring_view assetName)
{
	for(auto& obj : s_RenderObjects[assetName.data()])
	{
		//obj.SetTransform(position);
		obj.Draw(cmdList);
	}
}
