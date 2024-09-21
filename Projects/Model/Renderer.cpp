#include "Renderer.h"
#include "RenderObject.h"
#include "ResourceManager.h"
#include "Command.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "VertexBuffer.h"
#include "GameObject.h"

namespace
{
	std::map<std::wstring, std::vector<RenderObject>> s_RenderObjects;
	std::map<std::wstring, std::vector<Matrix4x4>> s_DrawList;
	std::mutex s_Mutex;

	VertexBuffer<Matrix4x4> instanceBuffer;
}

void Renderer::Load(std::wstring_view assetName, std::wstring_view modelName, std::wstring_view texturename, int32_t objectCount)
{
	auto model = ResourceManager::GetMesh(modelName);

	auto texture = ResourceManager::GetTexture(texturename);

	for(const auto& mesh : model.ModelMeshes)
	{
		auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();
		renderObject.Create(mesh, model.ModelMaterials.at(mesh.MaterialId), texture, objectCount);
	}
}

// positionのメモリを参照させる
void Add(GameObject& gameObject)
{
	//gameObject.transform = &instanceBuffer.at(0);
}

void Renderer::Load(std::wstring_view assetName, std::wstring_view modelName, int32_t objectCount)
{
	auto model = ResourceManager::GetMesh(modelName);

	for(const auto& mesh : model.ModelMeshes)
	{
		const auto& material = model.ModelMaterials.at(mesh.MaterialId);

		auto texture = ResourceManager::GetTexture((material.DiffuseMap.empty()) ? (L"pixel.png") : (material.DiffuseMap));
		auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();
		renderObject.Create(mesh, material, texture, objectCount);
	}
}

void Renderer::Draw(std::wstring_view assetName, Matrix4x4 matrixData)
{
	std::lock_guard<std::mutex> lock(s_Mutex);
	s_DrawList[assetName.data()].emplace_back(matrixData);
}

//void Renderer::Draw(std::wstring_view assetName, gsl::span<Matrix4x4> matrixData)
//{
//	for(auto& m : matrixData)
//		Draw(assetName, m);
//}

gsl::not_null<ID3D12GraphicsCommandList*> Renderer::Begin(const Camera& camera)
{
	s_DrawList.clear();

	for(auto& model : s_RenderObjects)
	{
		for(auto& mesh : model.second)
		{
			mesh.Initialize(camera);
		}
	}

	return Command::GetMainCmdList();
}

void Renderer::End(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	for(auto& model : s_RenderObjects)
	{
		for(auto& mesh : model.second)
		{
			auto& vec = s_DrawList[model.first];
			mesh.DrawArray(cmdList, vec);
		}
	}
}
