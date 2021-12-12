#include "TestModel.h"
#include "GraphicsCore.h"
#include "FileSearch.h"
#include "Display.h"
#include "ResourceManager.h"
#include "PipelineInitializer.h"


bool TestModel::OnInit()
{
	m_RotateAngle = static_cast<float>(rand());

	// 頂点データ生成
	{
		const Vertex data[] = {
				Vertex{DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
				Vertex{DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
				Vertex{DirectX::XMFLOAT3(1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
				Vertex{DirectX::XMFLOAT3(-1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)},
		};
		const auto span = gsl::make_span(data);

		m_VertexData.Create(sizeof(data), sizeof(Vertex));
		void* ptr = m_VertexData.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_VertexData.UnMap();
	}

	// インデックスデータ生成
	{
		const uint32_t data[] = { 0, 1, 2, 0, 2, 3 };
		const auto span = gsl::make_span(data);

		m_IndexData.Create(sizeof(data), sizeof(uint32_t));
		void* ptr = m_IndexData.Map();
		memcpy(ptr, span.data(), span.size_bytes());
		m_IndexData.UnMap();
	}

	// 定数バッファ生成
	{
		m_ConstantData.Create(sizeof(Transform), sizeof(Transform));
		m_pTransform = static_cast<Transform*>(m_ConstantData.Map());
	}

	m_CBV_SRVHeap.Create(2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	const auto& handle = m_CBV_SRVHeap.GetNextHandle();
	const auto constView = m_ConstantData.GetConstantView();
	Graphics::g_pDevice->CreateConstantBufferView(&constView, handle.CPU);

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
	m_pTransform->World *= DirectX::XMMatrixRotationY(m_RotateAngle);
	m_pTransform->World *= DirectX::XMMatrixTranslation(m_X,m_Y,m_Z);

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
	cmdList->SetGraphicsRootConstantBufferView(0, m_ConstantData.GetConstantView().BufferLocation);
	cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.m_HandleGPU);

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void TestModel::OnTerm() noexcept
{

}
