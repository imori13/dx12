#include "TestModel.h"
#include "GraphicsCore.h"
#include "WinApp.h"
#include "FileSearch.h"


bool TestModel::OnInit(const std::wstring& texturePath)
{
	m_RotateAngle = static_cast<float>(rand());
	HRESULT hr{};

	// 頂点データ生成
	{
		const Vertex data[] = {
				Vertex(DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)),
				Vertex(DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)),
				Vertex(DirectX::XMFLOAT3(1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)),
				Vertex(DirectX::XMFLOAT3(-1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)),
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

	// テクスチャバッファを生成
	m_Texture.CreateWIC(texturePath);

	// 定数バッファ用ヒープの生成
	m_CbvHeap.Create(2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	const auto constView = m_ConstantData.GetConstantView();
	Graphics::g_pDevice->CreateConstantBufferView(&constView, m_CbvHeap.GetCPUHandle(0));
	m_Texture.SetHeap(m_CbvHeap, 1);


	// ■ パイプラインステートの生成
	{
		// 入力レイアウトの設定 (頂点シェーダと定数バッファの紐づけ)
		D3D12_INPUT_ELEMENT_DESC elements[2];
		elements[0].SemanticName = "POSITION";
		elements[0].SemanticIndex = 0;	// POSITION0,1,2と頂点シェーダで使うなら設定する
		elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// フォーマット
		elements[0].InputSlot = 0;		// 複数の頂点バッファ用?
		elements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;	// 各要素が連続する場合のelement調整
		elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// 頂点ごとの入力データ
		elements[0].InstanceDataStepRate = 0;

		elements[1].SemanticName = "TEXCOORD";
		elements[1].SemanticIndex = 0;
		elements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		elements[1].InputSlot = 0;
		elements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		elements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elements[1].InstanceDataStepRate = 0;

		std::wstring vsPath;
		std::wstring psPath;

		if(!SearchFilePath(L"SimpleTexVS.cso", vsPath))
			ENSURES(false, "VertexShaderパス検索");

		if(!SearchFilePath(L"SimpleTexPS.cso", psPath))
			ENSURES(false, "PixelShaderパス検索");

		Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob;

		// 頂点シェーダ読み込み
		hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
		ENSURES(hr, "VertexShader読み込み");

		// ピクセルシェーダ読み込み
		hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
		ENSURES(hr, "PixelShader読み込み");

		// ルートシグネチャ読み込み
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		hr = D3DGetBlobPart(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &rootSignatureBlob);
		ENSURES(hr, "RootSignature設定の取得");

		// ルートシグネチャ設定
		hr = Graphics::g_pDevice->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		ENSURES(hr, "RootSignatureの生成");

		// パイプラインステート設定
		pipelineStateObject.SetInputLayout(gsl::make_span(elements));
		pipelineStateObject.SetRootSignature(m_pRootSignature.Get());
		pipelineStateObject.SetVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize());
		pipelineStateObject.SetPixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize());
		pipelineStateObject.SetRasterizerDesc();
		pipelineStateObject.SetBlendDesc();
		pipelineStateObject.SetDepthStencil(true);

		// パイプラインステート生成
		pipelineStateObject.Create();
	}

	// ■ ビューポートとシザー矩形の設定
	{
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = static_cast<float>(Window::g_Width);
		m_Viewport.Height = static_cast<float>(Window::g_Height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
		m_Scissor.left = 0;
		m_Scissor.right = Window::g_Width;
		m_Scissor.top = 0;
		m_Scissor.bottom = Window::g_Height;
	}

	auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
	auto targetPos = DirectX::XMVectorZero();
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
	const auto aspect = static_cast<float>(Window::g_Width) / static_cast<float>(Window::g_Height);

	m_pTransform->World = DirectX::XMMatrixIdentity();
	m_pTransform->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
	m_pTransform->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, 1.0f, 1000.0f);

	return true;
}

void TestModel::Update()
{
	m_pTransform->World = DirectX::XMMatrixRotationY(m_RotateAngle);
}

void TestModel::Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
	cmdList->SetPipelineState(pipelineStateObject.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto vertexView = m_VertexData.GetVertexView();
	cmdList->IASetVertexBuffers(0, 1, &vertexView);
	const auto indexView = m_IndexData.GetIndexView();
	cmdList->IASetIndexBuffer(&indexView);

	cmdList->SetDescriptorHeaps(1, m_CbvHeap.GetHeapAddress());
	cmdList->SetGraphicsRootConstantBufferView(0, m_ConstantData.GetConstantView().BufferLocation);
	cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.GetGpuHandle());

	cmdList->RSSetViewports(1, &m_Viewport);
	cmdList->RSSetScissorRects(1, &m_Scissor);
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void TestModel::OnTerm() noexcept
{

}
