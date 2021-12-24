#include "RenderObject.h"
#include "PipelineInitializer.h"
#include "Debug.h"
#include "Command.h"

void RenderObject::Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, uint32_t objectCount)
{
	// ヒープ生成
	{
		constexpr uint32_t materialCount = 1;
		constexpr uint32_t lightCount = 1;
		constexpr uint32_t textureCount = 1;
		const uint32_t heapCount = materialCount + lightCount + textureCount + objectCount;
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
		transformView.resize(objectCount);
		m_Transforms.resize(objectCount);
		m_TransformBuffer.Create(sizeof(Transform) * objectCount, sizeof(Transform));

		auto gpuVirtualAddress = reinterpret_cast<D3D12_GPU_VIRTUAL_ADDRESS>(m_TransformBuffer.Map());

		for(int64_t i = 0; i < gsl::narrow<int64_t>(objectCount); ++i)
		{
			// Descriptorのポインタを受け取る
			m_Transforms.at(i) = reinterpret_cast<Transform*>(gpuVirtualAddress);
			gpuVirtualAddress += sizeof(Transform);

			// ビュー設定
			auto& view = transformView.at(i);
			view.BufferLocation = m_TransformBuffer.GetConstantLocation() + sizeof(Transform) * i;
			view.SizeInBytes = sizeof(Transform);
			const auto handle = m_ResourceHeap.GetNextHandle();
			Graphics::g_pDevice->CreateConstantBufferView(&view, handle.CPU);
		}
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
		m_Bandles = Command::CreateBandle();

		m_Bandles->SetGraphicsRootSignature(pipeline.GetSignature());
		m_Bandles->SetPipelineState(pipeline.Get());
		m_Bandles->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_Bandles->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
		m_Bandles->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

		m_Bandles->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
		m_Bandles->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
		m_Bandles->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
		m_Bandles->SetGraphicsRootDescriptorTable(3, m_TextureGpuHandle);

		for(auto i = 0; i < objectCount; ++i)
		{
			m_Bandles->SetGraphicsRootConstantBufferView(0, transformView.at(i).BufferLocation);
			m_Bandles->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
		}
		m_Bandles->Close();
	}
}

void RenderObject::Initialize() noexcept
{
	m_DrawCount = 0;
}

void RenderObject::Draw(const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection, gsl::span<Vector3> positions)
{
#pragma omp parallel for
	for(int64_t i = 0; i < gsl::narrow<int64_t>(positions.size()); ++i)
	{
		auto instance = m_Transforms.at(i);
		instance->World = (world * Matrix4x4::Translate(positions[i])).Data();
		instance->View = view.Data();
		instance->Proj = projection.Data();
	}

	m_DrawCount = gsl::narrow<uint32_t>(positions.size());
}

void RenderObject::SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	const auto pipeline = PipelineInitializer::GetPipeline(L"DefaultPipeline");

	cmdList->SetGraphicsRootSignature(pipeline.GetSignature());
	cmdList->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
	cmdList->ExecuteBundle(m_Bandles);
}
