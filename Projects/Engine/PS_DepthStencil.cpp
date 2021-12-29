#include "PS_DepthStencil.h"

void PS_DepthStencil::Default() noexcept
{
	SetDepth(true);
	SetDepthFormat(DS_Format::Float);
	m_Desc.StencilEnable = false;
}

void PS_DepthStencil::SetDepthFormat(DS_Format format) noexcept
{
	m_Format = static_cast<DXGI_FORMAT>(format);
}

void PS_DepthStencil::SetDepth(bool flag) noexcept
{
	m_Desc.DepthEnable = flag;
	m_Desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	m_Desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}