#include "RenderObject.h"
#include "Debug.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "ResourceManager.h"
#include "Random.h"

bool RenderObject::DrawCollider = true;

void RenderObject::Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, int32_t objectCount)
{
	// ヒープ生成
	{
		constexpr uint32_t cameraCount = 1;
		constexpr uint32_t lightCount = 1;
		constexpr uint32_t textureCount = 1;
		constexpr uint32_t materialCount = 1;
		constexpr uint32_t colliderCount = 1;
		constexpr uint32_t heapCount = cameraCount + lightCount + colliderCount + materialCount + textureCount;
		m_ResourceHeap.Create(heapCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	// vertex
	m_VertexBuffer.Create(gsl::narrow<uint32_t>(mesh.Vertices.size()));
	m_VertexBuffer.MemCopy(mesh.Vertices);
	// instance
	m_InstanceBuffer.Create(objectCount);
	// index
	m_IndexBuffer.Create(gsl::narrow<uint32_t>(mesh.Indices.size()));
	m_IndexBuffer.MemCopy(mesh.Indices);

	// camera
	m_CameraBuffer.Create(m_ResourceHeap.GetNextHandle());
	// light
	m_LightBuffer.Create(m_ResourceHeap.GetNextHandle());
	// material
	m_MaterialBuffer.Create(m_ResourceHeap.GetNextHandle());
	m_MaterialBuffer.MemCopy(material);
	m_MaterialBuffer.data().Color = DirectX::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f);

	// Textureビュー設定
	{
		const auto handle = m_ResourceHeap.GetNextHandle();
		m_TextureGpuHandle = handle.GPU;

		Graphics::g_pDevice->CreateShaderResourceView(texture.Get(), &texture.GetView(), handle.CPU);
	}

	m_TexPipeline = ResourceManager::GetPipeline(L"NotTex");

	{
		m_TexBundle = Command::CreateBandle();

		m_TexBundle->SetGraphicsRootSignature(m_TexPipeline.RootSignature.Get());
		m_TexBundle->SetPipelineState(m_TexPipeline.PipelineState.Get());
		m_TexBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_TexBundle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetView());
		m_TexBundle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetView());
		m_TexBundle->IASetIndexBuffer(&m_IndexBuffer.GetView());

		m_TexBundle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_TexBundle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetGpuAddress());
		m_TexBundle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetGpuAddress());
		m_TexBundle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetGpuAddress());
		//m_TexBundle->SetGraphicsRootDescriptorTable(3, m_TextureGpuHandle);

		m_TexBundle->Close();
	}

	m_ColliderPipeline = ResourceManager::GetPipeline(L"Collider");

	{
		m_ColliderBundle = Command::CreateBandle();

		m_ColliderBundle->SetGraphicsRootSignature(m_ColliderPipeline.RootSignature.Get());
		m_ColliderBundle->SetPipelineState(m_ColliderPipeline.PipelineState.Get());
		m_ColliderBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_ColliderBundle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetView());
		m_ColliderBundle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetView());
		m_ColliderBundle->IASetIndexBuffer(&m_IndexBuffer.GetView());

		m_ColliderBundle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_ColliderBundle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetGpuAddress());
		m_ColliderBundle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetGpuAddress());
		m_ColliderBundle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetGpuAddress());

		m_ColliderBundle->Close();
	}
}

void RenderObject::Initialize(const Camera& camera) noexcept
{
	m_CameraBuffer.data().View = camera.GetViewMatrix().data();
	m_CameraBuffer.data().Proj = camera.GetProjMatrix().data();

	m_LightBuffer.data().LightDirection = DirectX::XMFLOAT3(Vector3::one().normalized().data());
	m_LightBuffer.data().LightColor = { 1.0f,1.0f,1.0f };
	m_LightBuffer.data().CameraPosition = camera.GetPosition().xmfloat3();
	m_LightBuffer.data().CameraDirection = camera.GetRotate().xmfloat3();
}

void RenderObject::Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData)
{
	const int32_t size = gsl::narrow<int32_t>(matrixData.size());

#pragma omp parallel for
	for(auto i = 0; i < size; ++i)
		m_InstanceBuffer.at(i) = matrixData[i].data();

	cmdList->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());

	cmdList->SetGraphicsRootSignature(m_TexPipeline.RootSignature.Get());
	cmdList->ExecuteBundle(m_TexBundle);
	cmdList->DrawIndexedInstanced(gsl::narrow<uint32_t>(m_IndexBuffer.size()), gsl::narrow_cast<uint32_t>(size), 0, 0, 0);

	if(DrawCollider)
	{
		cmdList->SetGraphicsRootSignature(m_ColliderPipeline.RootSignature.Get());
		cmdList->ExecuteBundle(m_ColliderBundle);
		cmdList->DrawIndexedInstanced(gsl::narrow<uint32_t>(m_IndexBuffer.size()), gsl::narrow_cast<uint32_t>(size), 0, 0, 0);
	}
}