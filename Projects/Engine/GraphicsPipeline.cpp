#include "GraphicsPipeline.h"
#include "GraphicsCore.h"
#include "Debug.h"

namespace
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_State;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> s_pPipelineStateObject;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> s_pRootSignature;
}

namespace PipelineCreater
{
	void SetShader(const ShaderData& shaderData)
	{
		s_pRootSignature = shaderData.ShaderSignature.Get();

		m_State.VS = shaderData.VertexShader.GetBytecode();
		m_State.PS = shaderData.PixelShader.GetBytecode();
		m_State.InputLayout = shaderData.ShaderInput.GetLayout();
		m_State.pRootSignature = shaderData.ShaderSignature.Get();
	}

	void SetRasterizerDesc() noexcept
	{
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

		m_State.RasterizerState = descRS;
	}

	void SetBlendDesc()
	{
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

		m_State.BlendState = descBS;
	}

	void SetDepthStencil(const bool enableDepth) noexcept
	{
		D3D12_DEPTH_STENCIL_DESC descDSS = {};
		descDSS.DepthEnable = enableDepth;
		descDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		descDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	// 深度テストの比較関係
		descDSS.StencilEnable = FALSE;

		m_State.DepthStencilState = descDSS;
	}

	GraphicsPipeline CreatePipeline()
	{
		m_State.SampleMask = UINT_MAX;
		m_State.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_State.NumRenderTargets = 1;
		m_State.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_State.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		m_State.SampleDesc.Count = 1;
		m_State.SampleDesc.Quality = 0;

		// パイプラインステートを生成
		const auto hr = Graphics::g_pDevice->CreateGraphicsPipelineState(&m_State, IID_PPV_ARGS(s_pPipelineStateObject.GetAddressOf()));
		ENSURES(hr, "PipelineStateObject生成");

		return GraphicsPipeline(s_pPipelineStateObject, s_pRootSignature);
	}
}
