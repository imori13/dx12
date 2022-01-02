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

	// 頂点情報Map
	{
		const auto span = gsl::make_span(mesh.Vertices);

		m_VertexBuffer.Create(span.size_bytes(), sizeof(ModelMeshVertex));
		void* ptr = m_VertexBuffer.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_VertexBuffer.UnMap();
	}

	// instanceMap
	{
		m_InstanceData.resize(objectCount);

		constexpr auto InstanceStrideSize = sizeof(DirectX::XMFLOAT4X4);
		m_InstanceBuffer.Create(InstanceStrideSize * objectCount, InstanceStrideSize);
		auto gpuVirtualAddress = reinterpret_cast<D3D12_GPU_VIRTUAL_ADDRESS>(m_InstanceBuffer.Map());
		for(auto i = 0; i < objectCount; ++i)
		{
			m_InstanceData.at(i) = reinterpret_cast<DirectX::XMFLOAT4X4*>(gpuVirtualAddress);

			gpuVirtualAddress += InstanceStrideSize;
		}
	}

	// Index情報Map
	{
		const auto span = gsl::make_span(mesh.Indices);
		m_IndexCount = gsl::narrow<int32_t>(span.size());

		m_IndexBuffer.Create(span.size_bytes(), sizeof(uint32_t));
		void* ptr = m_IndexBuffer.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_IndexBuffer.UnMap();
	}

	// CameraBuffer生成
	m_CameraBuffer.Create(m_ResourceHeap.GetNextHandle());
	// LightBuffer生成
	m_LightBuffer.Create(m_ResourceHeap.GetNextHandle());
	//  Material生成
	m_MaterialBuffer.Create(m_ResourceHeap.GetNextHandle());
	memcpy(m_MaterialBuffer.GetData(), &material, sizeof(material));

	// Textureビュー設定
	{
		const auto handle = m_ResourceHeap.GetNextHandle();
		m_TextureGpuHandle = handle.GPU;

		const auto textureView = texture.GetView();
		Graphics::g_pDevice->CreateShaderResourceView(texture.Get(), &textureView, handle.CPU);
	}

	// Outline
	m_OutlineBuffer.Create(m_ResourceHeap.GetNextHandle());
	m_OutlineBuffer.GetData()->OutlineColor = DirectX::XMFLOAT4{ 0.5f,1.0f,0.5f,1.0f };

	m_TexPipeline = ResourceManager::GetPipeline(L"NotTex");

	{
		m_TexBundle = Command::CreateBandle();

		m_TexBundle->SetGraphicsRootSignature(m_TexPipeline.RootSignature.Get());
		m_TexBundle->SetPipelineState(m_TexPipeline.PipelineState.Get());
		m_TexBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_TexBundle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_TexBundle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_TexBundle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_TexBundle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_TexBundle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetBufferLocation());
		m_TexBundle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetBufferLocation());
		m_TexBundle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetBufferLocation());
		//m_TexBundle->SetGraphicsRootDescriptorTable(3, m_TextureGpuHandle);

		m_TexBundle->Close();
	}

	m_ColliderPipeline = ResourceManager::GetPipeline(L"Collider");

	{
		m_ColliderBundle = Command::CreateBandle();

		m_ColliderBundle->SetGraphicsRootSignature(m_ColliderPipeline.RootSignature.Get());
		m_ColliderBundle->SetPipelineState(m_ColliderPipeline.PipelineState.Get());
		m_ColliderBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_ColliderBundle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_ColliderBundle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_ColliderBundle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_ColliderBundle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_ColliderBundle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetBufferLocation());
		m_ColliderBundle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetBufferLocation());
		m_ColliderBundle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetBufferLocation());

		m_ColliderBundle->Close();
	}
}

void RenderObject::Initialize(const Camera& camera) noexcept
{
	m_CameraBuffer.GetData()->View = camera.GetViewMatrix().data();
	m_CameraBuffer.GetData()->Proj = camera.GetProjMatrix().data();

	m_LightBuffer.GetData()->LightDirection = DirectX::XMFLOAT3(Vector3::one().normalized().data());
	m_LightBuffer.GetData()->LightColor = { 1.0f,1.0f,1.0f };
	m_LightBuffer.GetData()->CameraPosition = camera.GetPosition().xmfloat3();
	m_LightBuffer.GetData()->CameraDirection = camera.GetRotate().xmfloat3();
}

void RenderObject::Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData)
{
	const int32_t size = gsl::narrow<int32_t>(matrixData.size());

#pragma omp parallel for
	for(auto i = 0; i < size; ++i)
		*m_InstanceData.at(i) = matrixData[i].data();

	cmdList->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());

	cmdList->SetGraphicsRootSignature(m_TexPipeline.RootSignature.Get());
	cmdList->ExecuteBundle(m_TexBundle);
	cmdList->DrawIndexedInstanced(m_IndexCount, gsl::narrow_cast<uint32_t>(size), 0, 0, 0);

	if(DrawCollider)
	{
		cmdList->SetGraphicsRootSignature(m_ColliderPipeline.RootSignature.Get());
		cmdList->ExecuteBundle(m_ColliderBundle);
		cmdList->SetGraphicsRootConstantBufferView(3, m_OutlineBuffer.GetBufferLocation());
		cmdList->DrawIndexedInstanced(m_IndexCount, gsl::narrow_cast<uint32_t>(size), 0, 0, 0);
	}
}