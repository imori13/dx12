#include "RenderObject.h"
#include "Debug.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "ResourceManager.h"

void RenderObject::Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, int32_t objectCount)
{
	// ヒープ生成
	{
		constexpr uint32_t cameraCount = 1;
		constexpr uint32_t materialCount = 1;
		constexpr uint32_t lightCount = 1;
		constexpr uint32_t textureCount = 1;
		//const uint32_t heapCount = cameraCount + materialCount + lightCount + textureCount + objectCount;
		const uint32_t heapCount = cameraCount + materialCount + lightCount + textureCount + objectCount;
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
	{
		m_CameraBuffer.Create(sizeof(CameraData), sizeof(CameraData));
		auto gpuVirtualAddress = reinterpret_cast<D3D12_GPU_VIRTUAL_ADDRESS>(m_CameraBuffer.Map());
		m_CameraData = reinterpret_cast<CameraData*>(gpuVirtualAddress);

		m_CameraBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// LightBuffer生成
	{
		m_LightBuffer.Create(sizeof(LightData), sizeof(LightData));

		LightData light;
		light.LightPosition = { 0.0f,50.0f,100.0f,0.0f };
		light.Color = { 1.0f,1.0f,1.0f,0.0f };
		light.CameraPosition = { 0.0f,1.0f,2.0f,0.0f };

		void* ptr = m_LightBuffer.Map();
		memcpy(ptr, &light, sizeof(light));
		m_LightBuffer.UnMap();

		// ビュー設定
		m_LightBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// Material生成
	{
		m_MaterialBuffer.Create(sizeof(ModelMaterial), sizeof(ModelMaterial));

		void* ptr = m_MaterialBuffer.Map();
		memcpy(ptr, &material, sizeof(material));
		m_MaterialBuffer.UnMap();

		// ビュー設定
		m_MaterialBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// Textureビュー設定
	{
		const auto handle = m_ResourceHeap.GetNextHandle();
		m_TextureGpuHandle = handle.GPU;

		const auto textureView = texture.GetView();
		Graphics::g_pDevice->CreateShaderResourceView(texture.Get(), &textureView, handle.CPU);
	}

	m_Pipeline = ResourceManager::GetPipeline(L"DefaultPipeline");

	{
		m_Bandle = Command::CreateBandle();

		m_Bandle->SetGraphicsRootSignature(m_Pipeline.GetSignature());
		m_Bandle->SetPipelineState(m_Pipeline.GetPipeline());
		m_Bandle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_Bandle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_Bandle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_Bandle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_Bandle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_Bandle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetConstantLocation());
		m_Bandle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
		m_Bandle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
		m_Bandle->SetGraphicsRootDescriptorTable(3, m_TextureGpuHandle);

		m_Bandle->Close();
	}
}

void RenderObject::Initialize(const Matrix4x4& view, const Matrix4x4& projection) noexcept
{
	m_CameraData->View = view.data();
	m_CameraData->Proj = projection.data();
}

void RenderObject::Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData)
{
	const auto size = matrixData.size();

#pragma omp parallel for
	for(auto i = 0; i < size; ++i)
		*m_InstanceData.at(i) = matrixData[i].data();

	cmdList->SetGraphicsRootSignature(m_Pipeline.GetSignature());
	cmdList->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
	cmdList->ExecuteBundle(m_Bandle);
	cmdList->DrawIndexedInstanced(m_IndexCount, size, 0, 0, 0);
}