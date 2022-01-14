#include "Renderer.h"
#include "RenderObject.h"
#include "ResourceManager.h"
#include "Command.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"

namespace
{
	std::map<std::wstring, std::vector<RenderObject>> s_RenderObjects;
	std::map<std::wstring, std::vector<Matrix4x4>> s_DrawList;
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

void Renderer::Draw(std::wstring_view assetName, gsl::span<Matrix4x4> matrixData)
{
	for(auto& itr : matrixData)
		s_DrawList[assetName.data()].emplace_back(itr);
}

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

	const auto& cmdList = Command::BeginMain();

	// リソースバリア
	const auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	const D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = Display::g_DepthStencilBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// クリア
	const float clearColor[4] = { 0.05f,0.05f,0.05f,1.0f };
	cmdList->ClearRenderTargetView(rtvHandle, gsl::make_span(clearColor).data(), 0, nullptr);
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->RSSetViewports(1, &Display::g_Viewport);
	cmdList->RSSetScissorRects(1, &Display::g_Scissor);

	return cmdList;
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

	// リソースバリア
	const auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList->ResourceBarrier(1, &barrier);

	Command::EndMain();
}
