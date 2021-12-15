#include "TestModel.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "ResourceManager.h"
#include "PipelineInitializer.h"
#include "File.h"

bool TestModel::OnInit(std::wstring_view modelName)
{
	const Model& model = ResourceManager::GetModel(modelName);

	// 頂点データ生成
	{
		const std::vector<ModelMeshVertex> data = model.ModelMeshes.at(0).Vertices;
		const auto span = gsl::make_span(data);

		m_VertexData.Create(span.size_bytes(), sizeof(ModelMeshVertex));
		void* ptr = m_VertexData.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_VertexData.UnMap();
	}

	// インデックスデータ生成
	{
		const std::vector<uint32_t> data = ResourceManager::GetModel(modelName).ModelMeshes.at(0).Indices;
		const auto span = gsl::make_span(data);

		m_IndexCount = gsl::narrow<uint32_t>(span.size());

		m_IndexData.Create(span.size_bytes(), sizeof(uint32_t));
		void* ptr = m_IndexData.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_IndexData.UnMap();
	}

	// Transform生成
	{
		m_TransformData.Create(sizeof(Transform), sizeof(Transform));
		m_pTransform = static_cast<Transform*>(m_TransformData.Map());
	}

	// Light生成
	{
		m_LightData.Create(sizeof(Light), sizeof(Light));

		Light light;
		light.LightPosition = { 0.0f,50.0f,100.0f,0.0f };
		light.Color = { 1.0f,1.0f,1.0f,0.0f };
		light.CameraPosition = { 0.0f,1.0f,2.0f,0.0f };

		void* ptr = m_LightData.Map();
		memcpy(ptr, &light, sizeof(light));
		m_LightData.UnMap();
	}

	// Material生成
	{
		m_MaterialData.Create(sizeof(ModelMaterial), sizeof(ModelMaterial));

		const ModelMaterial material = model.ModelMaterials.at(0);

		void* ptr = m_MaterialData.Map();
		memcpy(ptr, &material, sizeof(material));
		m_MaterialData.UnMap();
	}

	m_CBV_SRVHeap.Create(4, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// TransformのBufferView
	auto handle = m_CBV_SRVHeap.GetNextHandle();
	const auto transformView = m_TransformData.GetConstantView();
	Graphics::g_pDevice->CreateConstantBufferView(&transformView, handle.CPU);

	// LightのBufferView
	handle = m_CBV_SRVHeap.GetNextHandle();
	const auto lightView = m_LightData.GetConstantView();
	Graphics::g_pDevice->CreateConstantBufferView(&lightView, handle.CPU);

	// MaterialのBufferView
	handle = m_CBV_SRVHeap.GetNextHandle();
	const auto materialView = m_MaterialData.GetConstantView();
	Graphics::g_pDevice->CreateConstantBufferView(&materialView, handle.CPU);


	auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
	auto targetPos = DirectX::XMVectorZero();
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);

	m_pTransform->World = DirectX::XMMatrixIdentity();
	m_pTransform->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
	m_pTransform->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, Display::g_Aspect, 1.0f, 1000.0f);

	return true;
}

void TestModel::SetTexture(const std::wstring_view textureName)
{
	m_Texture = ResourceManager::GetTexture(textureName);

	const auto& handle = m_CBV_SRVHeap.GetNextHandle();
	m_Texture.m_HandleCPU = handle.CPU;
	m_Texture.m_HandleGPU = handle.GPU;

	Graphics::g_pDevice->CreateShaderResourceView(m_Texture.Get(), &m_Texture.m_View, m_Texture.m_HandleCPU);
}

void TestModel::Update() noexcept
{
	m_pTransform->World = DirectX::XMMatrixIdentity();
	m_pTransform->World *= DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	m_pTransform->World *= DirectX::XMMatrixRotationZ(m_Rotate.z);
	m_pTransform->World *= DirectX::XMMatrixRotationY(m_Rotate.y);
	m_pTransform->World *= DirectX::XMMatrixRotationX(m_Rotate.x);
	m_pTransform->World *= DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
	m_pTransform->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, Display::g_Aspect, 1.0f, 1000.0f);
}

void TestModel::Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	const auto pipeline = PipelineInitializer::GetPipeline(L"DefaultPipeline");
	cmdList->SetGraphicsRootSignature(pipeline.GetSignature());
	cmdList->SetPipelineState(pipeline.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto vertexView = m_VertexData.GetVertexView();
	cmdList->IASetVertexBuffers(0, 1, &vertexView);
	const auto indexView = m_IndexData.GetIndexView();
	cmdList->IASetIndexBuffer(&indexView);

	cmdList->SetDescriptorHeaps(1, m_CBV_SRVHeap.GetAddress());
	cmdList->SetGraphicsRootConstantBufferView(0, m_TransformData.GetConstantView().BufferLocation);
	cmdList->SetGraphicsRootConstantBufferView(1, m_LightData.GetConstantView().BufferLocation);
	cmdList->SetGraphicsRootConstantBufferView(2, m_MaterialData.GetConstantView().BufferLocation);
	cmdList->SetGraphicsRootDescriptorTable(3, m_Texture.m_HandleGPU);

	cmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}

void TestModel::OnTerm() noexcept
{

}
