#include "PS_Rasterizer.h"

void PS_Rasterizer::Default() noexcept
{
	SetSolidMode(true);
	SetCulling(true);
	m_RasterizerDesc.FrontCounterClockwise = FALSE;			// 時計回り頂点を前面とする
	m_RasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;	// ピクセル深度調整。多分Zソート用
	m_RasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	m_RasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	m_RasterizerDesc.DepthClipEnable = FALSE;			// 距離に基づくクリッピング
	m_RasterizerDesc.MultisampleEnable = FALSE;		// マルチサンプリングon/off
	m_RasterizerDesc.AntialiasedLineEnable = FALSE;	// アンチエイリアシングon/off
	m_RasterizerDesc.ForcedSampleCount = 0;
	m_RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// ラスタライザの塗範囲広げる

	m_SampleDesc.Count = 1;
	m_SampleDesc.Quality = 0;

	m_SampleMask = UINT_MAX;
}

void PS_Rasterizer::SetSolidMode(bool flag) noexcept
{
	m_RasterizerDesc.FillMode = (flag) ? (D3D12_FILL_MODE_SOLID) : (D3D12_FILL_MODE_WIREFRAME);
}

void PS_Rasterizer::SetCulling(bool flag) noexcept
{
	m_RasterizerDesc.CullMode = (flag) ? (D3D12_CULL_MODE_BACK) : (D3D12_CULL_MODE_NONE);
}

void PS_Rasterizer::SetMultiSample(bool flag, uint32_t count, uint32_t quality) noexcept
{
	m_RasterizerDesc.MultisampleEnable = flag;
	m_SampleDesc.Count = count;
	m_SampleDesc.Quality = quality;
}
