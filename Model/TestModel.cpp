#include "TestModel.h"
#include "GraphicsCore.h"
#include "WinApp.h"
#include "FileSearch.h"

#include <d3dcompiler.h>

bool TestModel::OnInit()
{
	m_RotateAngle = static_cast<float>(rand());
	HRESULT hr{};

	// 頂点データ
	const Vertex vertices[] = {
			Vertex(DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)),
			Vertex(DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)),
			Vertex(DirectX::XMFLOAT3(1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)),
			Vertex(DirectX::XMFLOAT3(-1.0f,-1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)),
	};
	// インデックスデータ
	const uint32_t index[] = { 0, 1, 2, 0, 2, 3 };

	const auto vertexSpan = gsl::make_span(vertices);
	const auto indexSpan = gsl::make_span(index);

	const auto vertexSize = vertexSpan.size_bytes();
	const auto indexSize = indexSpan.size_bytes();
	constexpr auto constSize = sizeof(Transform);

	constexpr auto bufferSize = vertexSize + indexSize + constSize;

	// バッファを生成
	m_UploadBuffer.Create(bufferSize);

	auto gpuVirtualAddress = reinterpret_cast<D3D12_GPU_VIRTUAL_ADDRESS>(m_UploadBuffer.Map());
	m_pTransform = gsl::not_null(reinterpret_cast<Transform*>(gpuVirtualAddress));

	memcpy(reinterpret_cast<void*>(gpuVirtualAddress), m_pTransform, constSize);
	gpuVirtualAddress += constSize;
	memcpy(reinterpret_cast<void*>(gpuVirtualAddress), vertexSpan.data(), vertexSize);
	gpuVirtualAddress += vertexSize;
	memcpy(reinterpret_cast<void*>(gpuVirtualAddress), indexSpan.data(), indexSize);
	gpuVirtualAddress += indexSize;


	m_UploadBuffer.CreateConstantView(0, constSize);
	m_UploadBuffer.CreateVertexView(constSize, vertexSize, sizeof(Vertex));
	m_UploadBuffer.CreateIndexView(constSize + vertexSize, indexSize, DXGI_FORMAT_R32_UINT);
	m_Texture.Create(L"Resource/Texture/neko.dds");

	// 定数バッファ用ヒープの生成
	m_CbvHeap.Create(2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	Graphics::g_pDevice->CreateConstantBufferView(&m_UploadBuffer.GetConstantView(), m_CbvHeap.GetCPUHandle(0));
	m_Texture.SetHeap(m_CbvHeap, 1);


	// ■ ルートシグニチャの生成
	{
		// ルートシグネチャが頂点シェーダ―のみを使用するので、最適化しやすいようにフラグを設定
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// SRV用
		D3D12_DESCRIPTOR_RANGE range = {};
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = 0;

		// ルートパラメータの設定
		D3D12_ROOT_PARAMETER rootparam[2] = {};
		rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootparam[0].Descriptor.ShaderRegister = 0;
		rootparam[0].Descriptor.RegisterSpace = 0;
		rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootparam[1].DescriptorTable.NumDescriptorRanges = 1;
		rootparam[1].DescriptorTable.pDescriptorRanges = &range;
		rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// スタティックサンプラーの設定
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		sampler.MinLOD = -D3D12_FLOAT32_MAX;
		sampler.MaxLOD = +D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC rootdesc = {};
		rootdesc.NumParameters = 2;
		rootdesc.NumStaticSamplers = 1;
		rootdesc.pParameters = static_cast<D3D12_ROOT_PARAMETER*>(rootparam);
		rootdesc.pStaticSamplers = &sampler;
		rootdesc.Flags = flag;

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;

		// シリアライズ (直列化)
		hr = D3D12SerializeRootSignature(
			&rootdesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,
			pBlob.GetAddressOf(),
			pErrorBlob.GetAddressOf());
		ENSURES(hr, "RootSignature直列化")

			// ルートシグニチャ
			hr = Graphics::g_pDevice->CreateRootSignature(
				0,
				pBlob->GetBufferPointer(),
				pBlob->GetBufferSize(),
				IID_PPV_ARGS(m_pRootSignature.GetAddressOf()));
		ENSURES(hr, "RootSignature生成")
	}

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

		// ラスタライザーステートの設定
		D3D12_RASTERIZER_DESC descRS;
		descRS.FillMode = D3D12_FILL_MODE_SOLID;		// ワイヤーフレームor塗りつぶし
		descRS.CullMode = D3D12_CULL_MODE_NONE;			// 裏面カリングon/off
		descRS.FrontCounterClockwise = FALSE;			// 時計回り頂点を前面とする
		descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;	// ピクセル深度調整。多分Zソート用
		descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		descRS.DepthClipEnable = FALSE;			// 距離に基づくクリッピング
		descRS.MultisampleEnable = FALSE;		// マルチサンプリングon/off
		descRS.AntialiasedLineEnable = FALSE;	// アンチエイリアシングon/off
		descRS.ForcedSampleCount = 0;
		descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// ラスタライザの塗範囲広げる

		// レンダーターゲットのブレンド設定(半透明などが可能)
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		// ブレンドステートの設定
		D3D12_BLEND_DESC descBS;
		descBS.AlphaToCoverageEnable = FALSE;
		descBS.IndependentBlendEnable = FALSE;
		for(UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{ gsl::at(descBS.RenderTarget, i) = descRTBS; }

		// 深度ステンシルステートの設定
		D3D12_DEPTH_STENCIL_DESC descDSS = {};
		descDSS.DepthEnable = TRUE;
		descDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		descDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	// 深度テストの比較関係
		descDSS.StencilEnable = FALSE;

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

		// パイプラインステートの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineState = {};
		const auto span = gsl::make_span(elements);
		pipelineState.InputLayout = { span.data(), gsl::narrow<uint32_t>(span.size()) };
		pipelineState.pRootSignature = m_pRootSignature.Get();
		pipelineState.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
		pipelineState.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
		pipelineState.RasterizerState = descRS;
		pipelineState.BlendState = descBS;
		pipelineState.DepthStencilState = descDSS;
		pipelineState.SampleMask = UINT_MAX;
		pipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineState.NumRenderTargets = 1;
		pipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		pipelineState.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineState.SampleDesc.Count = 1;
		pipelineState.SampleDesc.Quality = 0;

		// パイプラインステートを生成
		hr = Graphics::g_pDevice->CreateGraphicsPipelineState(
			&pipelineState,
			IID_PPV_ARGS(m_pPSO.GetAddressOf()));
		ENSURES(hr, "PipelineStateObject生成")
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
	// Transformの更新
	m_RotateAngle += 0.025f;
	m_pTransform->World = DirectX::XMMatrixRotationY(m_RotateAngle);
}

void TestModel::Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList)
{
	cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
	cmdList->SetDescriptorHeaps(1, m_CbvHeap.GetHeapAddress());
	cmdList->SetPipelineState(m_pPSO.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &m_UploadBuffer.GetVertexView());
	cmdList->IASetIndexBuffer(&m_UploadBuffer.GetIndexView());
	cmdList->SetGraphicsRootConstantBufferView(0, m_UploadBuffer.GetConstantView().BufferLocation);
	cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.GetGpuHandle());
	cmdList->RSSetViewports(1, &m_Viewport);
	cmdList->RSSetScissorRects(1, &m_Scissor);
	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void TestModel::OnTerm() noexcept
{

}
