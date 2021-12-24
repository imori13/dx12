#include "RenderObject.h"
#include "PipelineInitializer.h"
#include "Debug.h"
#include "Command.h"

void RenderObject::Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, uint32_t objectCount)
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

		constexpr auto InstanceStrideSize = sizeof(DirectX::XMFLOAT3);
		m_InstanceBuffer.Create(InstanceStrideSize * objectCount, InstanceStrideSize);
		auto gpuVirtualAddress = reinterpret_cast<D3D12_GPU_VIRTUAL_ADDRESS>(m_InstanceBuffer.Map());
		for(auto i = 0; i < objectCount; ++i)
		{
			m_InstanceData.at(i) = reinterpret_cast<DirectX::XMFLOAT3*>(gpuVirtualAddress);

			gpuVirtualAddress += InstanceStrideSize;
		}
	}

	// Index情報Map
	{
		const auto span = gsl::make_span(mesh.Indices);

		m_IndexCount = gsl::narrow<uint32_t>(span.size());

		m_IndexBuffer.Create(span.size_bytes(), sizeof(uint32_t));
		void* ptr = m_IndexBuffer.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_IndexBuffer.UnMap();
	}

	// Transform生成
	{
		m_TransformBuffer.Create(sizeof(Transform), sizeof(Transform));
		auto gpuVirtualAddress = reinterpret_cast<D3D12_GPU_VIRTUAL_ADDRESS>(m_TransformBuffer.Map());
		m_Transform = reinterpret_cast<Transform*>(gpuVirtualAddress);

		m_TransformBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
	}

	// Light生成
	{
		m_LightBuffer.Create(sizeof(Light), sizeof(Light));

		Light light;
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

	const auto threadNum = omp_get_thread_num();

	{
		const auto pipeline = PipelineInitializer::GetPipeline(L"DefaultPipeline");
		m_Bandle = Command::CreateBandle();

		m_Bandle->SetGraphicsRootSignature(pipeline.GetSignature());
		m_Bandle->SetPipelineState(pipeline.Get());
		m_Bandle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_Bandle->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_Bandle->IASetVertexBuffers(1, 1, &m_InstanceBuffer.GetVertexView());
		m_Bandle->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_Bandle->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_Bandle->SetGraphicsRootConstantBufferView(0, m_TransformBuffer.GetConstantLocation());
		m_Bandle->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
		m_Bandle->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
		m_Bandle->SetGraphicsRootDescriptorTable(3, m_TextureGpuHandle);
		m_Bandle->DrawIndexedInstanced(m_IndexCount, objectCount, 0, 0, 0);

		m_Bandle->Close();
	}
}

void RenderObject::Initialize() noexcept
{

}

void RenderObject::Draw(const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection, const gsl::span<Vector3>& positions)
{
	m_DrawCount = gsl::narrow<uint32_t>(positions.size());

	m_Transform->World = (Matrix4x4::Identity() * Matrix4x4::Translate(Vector3(0))).Data();
	m_Transform->View = view.Data();
	m_Transform->Proj = projection.Data();

	for(auto i = 0; i < m_DrawCount; ++i)
	{
		//const auto vec = (world * Matrix4x4::Translate(positions[i])) * Vector3::One();
		const auto vec = Matrix4x4::Translate(positions[i]) * Vector3::One();
		*m_InstanceData.at(i) = DirectX::XMFLOAT3{ vec.x(), vec.y(), vec.z() };
	}
}

void RenderObject::SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	const auto pipeline = PipelineInitializer::GetPipeline(L"DefaultPipeline");

	cmdList->SetGraphicsRootSignature(pipeline.GetSignature());
	cmdList->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
	cmdList->ExecuteBundle(m_Bandle);
}