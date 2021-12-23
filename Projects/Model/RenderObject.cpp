#include "RenderObject.h"
#include "PipelineInitializer.h"
#include "Debug.h"

void RenderObject::Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, uint32_t objectCount)
{
	m_ObjectCount = objectCount;

	// ヒープ生成
	{
		constexpr uint32_t materialCount = 1;
		constexpr uint32_t lightCount = 1;
		constexpr uint32_t cameraCount = 1;
		constexpr uint32_t textureCount = 1;
		const uint32_t heapCount = materialCount + cameraCount + lightCount + textureCount + objectCount;
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
		//		m_TransformBuffers.resize(objectCount);
		//		m_Transforms.resize(objectCount);
		//
		//#pragma omp parallel for
		//		for(int64_t i = 0; i < gsl::narrow<int64_t>(objectCount); ++i)
		//		{
		//			auto& buffer = m_TransformBuffers.at(i);
		//
		//			buffer.Create(sizeof(Transform), sizeof(Transform));
		//			Transform* transform = static_cast<Transform*>(buffer.Map());
		//
		//			m_Transforms.at(i) = transform;
		//
		//			// ビュー設定
		//			buffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
		//		}
		//#pragma omp barrier

		transformView.resize(objectCount);
		m_Transforms.resize(objectCount);
		TransformBuffer.Create(sizeof(Transform) * objectCount, sizeof(Transform));
		auto gpuVirtualAddress = reinterpret_cast<D3D12_GPU_VIRTUAL_ADDRESS>(TransformBuffer.Map());

		for(int64_t i = 0; i < gsl::narrow<int64_t>(objectCount); ++i)
		{
			m_Transforms.at(i) = reinterpret_cast<Transform*>(gpuVirtualAddress);
			auto& view = transformView.at(i);
			gpuVirtualAddress += sizeof(Transform);

			view.BufferLocation = TransformBuffer.GetConstantLocation() + sizeof(Transform) * i;
			view.SizeInBytes = sizeof(Transform);
			const auto handle = m_ResourceHeap.GetNextHandle();
			Graphics::g_pDevice->CreateConstantBufferView(&view, handle.CPU);
		}
	}

	// カメラ生成
	{
		m_CameraBuffer.Create(sizeof(CameraBuff), sizeof(CameraBuff));

		m_CameraData = static_cast<CameraBuff*>(m_CameraBuffer.Map());

		// ビュー設定
		m_CameraBuffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
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
}

void RenderObject::Initialize()
{
	m_DrawIndex = 0;

	//for(const auto& drawed : m_WaitDraw)
	//{
	//	m_Transforms.emplace_back(drawed);
	//}
	//m_WaitDraw.clear();
}

void RenderObject::Draw(const Matrix4x4& world, const Matrix4x4& view, const Matrix4x4& projection, gsl::span<Vector3> positions)
{
#ifdef _DEBUG
	if(m_DrawIndex >= m_ObjectCount)
	{
		LOGLINE(L"WARNING 描画呼び出し制限に達したので、描画を無視しました");
		return;
	}
#endif		

	m_CameraData->View = view.Data();
	m_CameraData->Proj = projection.Data();

#pragma omp parallel for
	for(int64_t i = 0; i < positions.size(); ++i)
	{
		auto instance = m_Transforms.at(i);
		instance->World = (world * Matrix4x4::Translate(positions[i])).Data();
	}
#pragma omp barrier

	m_DrawIndex = positions.size();
}

void RenderObject::SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	const auto pipeline = PipelineInitializer::GetPipeline(L"DefaultPipeline");
	cmdList->SetGraphicsRootSignature(pipeline.GetSignature());
	cmdList->SetPipelineState(pipeline.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->IASetVertexBuffers(0, 1, &m_VertexBuffer.GetVertexView());
	cmdList->IASetIndexBuffer(&m_IndexBuffer.GetIndexView());

	cmdList->SetDescriptorHeaps(1, m_ResourceHeap.GetAddress());
	cmdList->SetGraphicsRootConstantBufferView(1, m_CameraBuffer.GetConstantLocation());
	cmdList->SetGraphicsRootConstantBufferView(2, m_LightBuffer.GetConstantLocation());
	cmdList->SetGraphicsRootConstantBufferView(3, m_MaterialBuffer.GetConstantLocation());
	cmdList->SetGraphicsRootDescriptorTable(4, m_TextureGpuHandle);

	for(auto i = 0u; i < m_DrawIndex; ++i)
	{
		cmdList->SetGraphicsRootConstantBufferView(0, transformView.at(i).BufferLocation);
		cmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
	}
}
