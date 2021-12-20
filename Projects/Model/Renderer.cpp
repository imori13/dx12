#include "Renderer.h"
#include "ResourceManager.h"
#include "Command.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"

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

		auto textureName = (!material.DiffuseMap.empty()) ? (material.DiffuseMap) : (L"body_tex.tga");
		auto texture = ResourceManager::GetTexture(textureName);

		auto& renderObject = s_RenderObjects[assetName.data()].emplace_back();
		renderObject.Create(mesh, material, texture, 100);
	}
}

gsl::not_null<ID3D12GraphicsCommandList*> Renderer::Begin()
{
	auto cmdList = Command::Begin();

	// ���\�[�X�o���A
	auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &barrier);

	// �����_�[�^�[�Q�b�g�̐ݒ�
	const auto rtvHandle = Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	const auto dsvHandle = Display::g_DepthStencilBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// �N���A
	const float clearColor[4] = { 0.05f,0.05f,0.05f,1.0f };
	cmdList->ClearRenderTargetView(rtvHandle, gsl::make_span(clearColor).data(), 0, nullptr);
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->RSSetViewports(1, &Display::g_Viewport);
	cmdList->RSSetScissorRects(1, &Display::g_Scissor);

	return cmdList;
}

void Renderer::Draw(std::wstring_view assetName, const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection)
{
	auto& meshVec = s_RenderObjects[assetName.data()];

	for(auto& mesh : meshVec)
	{
		mesh.Draw(world, view, projection);
	}
}

void Renderer::Draw(std::wstring_view assetName, const Matrix4x4& world, const Camera& camera)
{
	Draw(assetName, world, camera.GetViewMatrix(), camera.GetProjMatrix());
}

void Renderer::SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	for(auto& model : s_RenderObjects)
	{
		for(auto& mesh : model.second)
		{
			mesh.SendCommand(cmdList);
			mesh.Initialize();
		}
	}
}

void Renderer::End(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	// ���\�[�X�o���A
	auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList->ResourceBarrier(1, &barrier);

	Command::End();
}
