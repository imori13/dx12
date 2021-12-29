#include "RenderObject.h"
#include "Debug.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "ResourceManager.h"
#include "Random.h"

bool RenderObject::DrawCollider = false;

void RenderObject::Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, int32_t objectCount)
{
	// ヒープ生成
	{
		constexpr uint32_t cameraCount = 1;
		constexpr uint32_t lightCount = 1;
		constexpr uint32_t textureCount = 1;
		constexpr uint32_t materialCount = 1;
		constexpr uint32_t colliderCount = 1;
		const uint32_t heapCount = cameraCount + lightCount + colliderCount + materialCount + textureCount + objectCount;
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
		m_CameraData = static_cast<CameraData*>(m_CameraBuffer.Map());

		m_CameraBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// LightBuffer生成
	{
		m_LightBuffer.Create(sizeof(LightData), sizeof(LightData));
		m_LightData = static_cast<LightData*>(m_LightBuffer.Map());

		// ビュー設定
		m_LightBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	//  Material生成
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

	// Collider
	{
		m_ColliderBuffer.Create(256, 256);
		DirectX::XMFLOAT4* collider = static_cast<DirectX::XMFLOAT4*>(m_ColliderBuffer.Map());
		*collider = DirectX::XMFLOAT4{ 0.5f,1.0f,0.5f,1.0f };

		// ビュー設定
		m_ColliderBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	m_TexPipeline = ResourceManager::GetPipeline(L"Tex");

	{
		m_TexBandle = Command::CreateBandle();
		m_TexBandle->SetGraphicsRootSignature(m_TexPipeline.Signature.Get());
		m_TexBandle->SetPipelineState(m_TexPipeline.Pipeine.Get());
		m_TexBandle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_TexBandle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_TexBandle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_TexBandle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_TexBandle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_TexBandle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetConstantLocation());
		m_TexBandle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
		m_TexBandle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
		m_TexBandle->SetGraphicsRootDescriptorTable(3, m_TextureGpuHandle);

		m_TexBandle->Close();
	}

	m_ColliderPipeline = ResourceManager::GetPipeline(L"Collider");
	{
		m_ColliderBandle = Command::CreateBandle();
		m_ColliderBandle->SetGraphicsRootSignature(m_ColliderPipeline.Signature.Get());
		m_ColliderBandle->SetPipelineState(m_ColliderPipeline.Pipeine.Get());
		m_ColliderBandle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_ColliderBandle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_ColliderBandle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_ColliderBandle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_ColliderBandle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetConstantLocation());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(3, m_ColliderBuffer.GetConstantLocation());

		m_ColliderBandle->Close();
	}
}

void RenderObject::Create(const ModelMesh& mesh, int32_t objectCount)
{
	// ヒープ生成
	{
		constexpr uint32_t cameraCount = 1;
		constexpr uint32_t lightCount = 1;
		constexpr uint32_t materialCount = 1;
		constexpr uint32_t colliderCount = 1;
		const uint32_t heapCount = cameraCount + lightCount + colliderCount + materialCount + objectCount;
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
		m_CameraData = static_cast<CameraData*>(m_CameraBuffer.Map());

		m_CameraBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// LightBuffer生成
	{
		m_LightBuffer.Create(sizeof(LightData), sizeof(LightData));
		m_LightData = static_cast<LightData*>(m_LightBuffer.Map());

		// ビュー設定
		m_LightBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// Material
	{
		m_MaterialBuffer.Create(sizeof(ModelMaterial), sizeof(ModelMaterial));
		ModelMaterial* material = static_cast<ModelMaterial*>(m_MaterialBuffer.Map());

		float a = 0.2f;
		material->Ambient = { a,a,a };
		a = 0.8f;
		material->Diffuse = { a,a,a };
		a = 0.25f;
		material->Specular = { a,a,a };
		material->Alpha = 1.0f;
		material->Shininess = 1.0f;

		// ビュー設定
		m_MaterialBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// NotTex
	{
		m_NotTexBuffer.Create(256, 256);
		DirectX::XMFLOAT4* float4 = static_cast<DirectX::XMFLOAT4*>(m_NotTexBuffer.Map());
		*float4 = DirectX::XMFLOAT4{ 0.75f,0.75f,0.75f,1.0f };

		// ビュー設定
		m_NotTexBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// Collider
	{
		m_ColliderBuffer.Create(256, 256);
		DirectX::XMFLOAT4* collider = static_cast<DirectX::XMFLOAT4*>(m_ColliderBuffer.Map());
		*collider = DirectX::XMFLOAT4{ 0.5f,1.0f,0.5f,1.0f };

		// ビュー設定
		m_ColliderBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	m_TexPipeline = ResourceManager::GetPipeline(L"NotTex");

	{
		m_TexBandle = Command::CreateBandle();
		m_TexBandle->SetGraphicsRootSignature(m_TexPipeline.Signature.Get());
		m_TexBandle->SetPipelineState(m_TexPipeline.Pipeine.Get());
		m_TexBandle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_TexBandle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_TexBandle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_TexBandle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_TexBandle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_TexBandle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetConstantLocation());
		m_TexBandle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
		m_TexBandle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
		m_TexBandle->SetGraphicsRootConstantBufferView(3, m_NotTexBuffer.GetConstantLocation());

		m_TexBandle->Close();
	}

	m_ColliderPipeline = ResourceManager::GetPipeline(L"Collider");
	{
		m_ColliderBandle = Command::CreateBandle();
		m_ColliderBandle->SetGraphicsRootSignature(m_ColliderPipeline.Signature.Get());
		m_ColliderBandle->SetPipelineState(m_ColliderPipeline.Pipeine.Get());
		m_ColliderBandle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_ColliderBandle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_ColliderBandle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_ColliderBandle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_ColliderBandle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(0, m_CameraBuffer.GetConstantLocation());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
		m_ColliderBandle->SetGraphicsRootConstantBufferView(3, m_ColliderBuffer.GetConstantLocation());

		m_ColliderBandle->Close();
	}
}

void RenderObject::Initialize(const Camera& camera) noexcept
{
	m_CameraData->View = camera.GetViewMatrix().data();
	m_CameraData->Proj = camera.GetProjMatrix().data();

	m_LightData->LightDirection = DirectX::XMFLOAT3(Vector3::right().normalized().data());
	m_LightData->LightColor = { 1.0f,1.0f,1.0f };
	m_LightData->CameraPosition = camera.GetPosition().xmfloat3();
	m_LightData->CameraDirection = camera.GetRotate().xmfloat3();
}

void RenderObject::Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData)
{
	const int32_t size = gsl::narrow<int32_t>(matrixData.size());

#pragma omp parallel for
	for(auto i = 0; i < size; ++i)
		*m_InstanceData.at(i) = matrixData[i].data();

	cmdList->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());

	cmdList->SetGraphicsRootSignature(m_TexPipeline.Signature.Get());
	cmdList->ExecuteBundle(m_TexBandle);
	cmdList->DrawIndexedInstanced(m_IndexCount, gsl::narrow_cast<uint32_t>(size), 0, 0, 0);

	if(DrawCollider)
	{
		cmdList->SetGraphicsRootSignature(m_ColliderPipeline.Signature.Get());
		cmdList->ExecuteBundle(m_ColliderBandle);
		cmdList->DrawIndexedInstanced(m_IndexCount, gsl::narrow_cast<uint32_t>(size), 0, 0, 0);
	}
}