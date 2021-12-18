#include "RenderObject.h"

#include <Display.h>	// 後で消す
#include "PipelineInitializer.h"

void RenderObject::Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, uint32_t objectCount)
{
	m_ObjectCount = objectCount;

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

		const auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
		const auto targetPos = DirectX::XMVectorZero();
		const auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);

		m_TransformBuffers.resize(objectCount);

		for(auto i = 0u; i < objectCount; ++i)
		{
			auto& buffer = m_TransformBuffers.at(i);

			buffer.Create(sizeof(Transform), sizeof(Transform));
			Transform* transform = static_cast<Transform*>(buffer.Map());

			m_Transforms.emplace_back(transform);

			transform->World = DirectX::XMMatrixIdentity();
			transform->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
			transform->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, Display::g_Aspect, 1.0f, 1000.0f);

			transform += sizeof(Transform);

			// ビュー設定
			buffer.CreateConstantView(m_ResourceHeap.GetNextHandle());
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
}

void RenderObject::Initialize()
{
	m_DrawIndex = 0;

	for(const auto& drawed : m_WaitDraw)
	{
		m_Transforms.emplace_back(drawed);
	}
	m_WaitDraw.clear();
}

void RenderObject::Draw(
	DirectX::XMFLOAT3 pos,
	DirectX::XMFLOAT3 rotate,
	DirectX::XMFLOAT3 scale)
{
	if(m_DrawIndex >= m_ObjectCount)
	{
		LOGLINE(L"WARNING 描画呼び出し制限に達したので、描画を無視しました");
		return;
	}

	auto instance = m_Transforms.at(m_DrawIndex);

	instance->World = DirectX::XMMatrixIdentity();
	instance->World *= DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	instance->World *= DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);
	instance->World *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
	instance->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, Display::g_Aspect, 1.0f, 1000.0f);

	++m_DrawIndex;
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
	cmdList->SetGraphicsRootConstantBufferView(1, m_LightBuffer.GetConstantLocation());
	cmdList->SetGraphicsRootConstantBufferView(2, m_MaterialBuffer.GetConstantLocation());
	cmdList->SetGraphicsRootDescriptorTable(3, m_TextureGpuHandle);

	for(auto i = 0u; i < m_DrawIndex; ++i)
	{
		cmdList->SetGraphicsRootConstantBufferView(0, m_TransformBuffers.at(i).GetConstantLocation());

		cmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
	}
}
