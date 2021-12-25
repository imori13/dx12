#include "Renderer.h"
#include "ResourceManager.h"
#include "Command.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"

namespace
{
	std::map<std::wstring, std::vector<RenderObject>> s_RenderObjects;
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

		auto textureName = (!material.DiffuseMap.empty()) ? (material.DiffuseMap) : (L"body_tex.tga");
		auto texture = ResourceManager::GetTexture(textureName);

		auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();
		renderObject.Create(mesh, material, texture, objectCount);
	}
}

void Renderer::Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, std::wstring_view assetName, gsl::span<Matrix4x4> matrixData)
{
	auto& meshVec = s_RenderObjects[assetName.data()];
	for(auto& mesh : meshVec)
	{
		mesh.Draw(cmdList, matrixData);
	}
}

gsl::not_null<ID3D12GraphicsCommandList*> Renderer::Begin(Matrix4x4 view, Matrix4x4 proj)
{
	for(auto& model : s_RenderObjects)
	{
		for(auto& mesh : model.second)
		{
			mesh.Initialize(view, proj);
		}
	}

	auto cmdList = Command::BeginMain();

	// リソースバリア
	auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	const auto rtvHandle = Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	const auto dsvHandle = Display::g_DepthStencilBuffer.at(Display::g_FrameIndex).GetCpuHandle();
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
	// リソースバリア
	auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList->ResourceBarrier(1, &barrier);

	Command::EndMain();
}
