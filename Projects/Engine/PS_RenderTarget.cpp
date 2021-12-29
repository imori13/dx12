#include "PS_RenderTarget.h"

namespace
{
	constexpr D3D12_RENDER_TARGET_BLEND_DESC ToDesc(RTV_Blend blend) noexcept
	{
		D3D12_RENDER_TARGET_BLEND_DESC retVal = {};

		switch(blend)
		{
			case RTV_Blend::None:
			{
				retVal.BlendEnable = false;
				retVal.LogicOpEnable = false;
				retVal.SrcBlend = D3D12_BLEND_ONE;
				retVal.DestBlend = D3D12_BLEND_ZERO;
				retVal.BlendOp = D3D12_BLEND_OP_ADD;
				retVal.SrcBlendAlpha = D3D12_BLEND_ONE;
				retVal.DestBlendAlpha = D3D12_BLEND_ZERO;
				retVal.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				retVal.LogicOp = D3D12_LOGIC_OP_NOOP;
				retVal.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
				break;
			}

			default:
			{
				break;
			}
		}

		return retVal;
	}
}

void PS_RenderTarget::Default() noexcept(false)
{
	for(uint8_t i = 0; i < MAX_RTV_COUNT; ++i)
	{
		SetRtvFormat(i, RTV_Format::Float4);
		SetRtvBlend(i, RTV_Blend::None);
	}

	// ブレンドステートの設定
	m_BlendDesc.AlphaToCoverageEnable = FALSE;
	m_BlendDesc.IndependentBlendEnable = FALSE;

	m_RtvNum = 1;
}

void PS_RenderTarget::SetRtvFormat(uint8_t rtvIndex, RTV_Format format)
{
	m_RtvFormat.at(rtvIndex) = static_cast<DXGI_FORMAT>(format);
}

void PS_RenderTarget::SetRtvBlend(uint8_t rtvIndex, RTV_Blend blend)
{
	gsl::at(m_BlendDesc.RenderTarget, rtvIndex) = ToDesc(blend);
}

void PS_RenderTarget::SetRtvNum(uint8_t rtvNum) noexcept
{
	m_RtvNum = rtvNum;
}
