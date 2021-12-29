#pragma once

enum class DS_Format :uint8_t;

class PS_DepthStencil
{
public:
	PS_DepthStencil() noexcept
	{
		Default();
	}

	void SetDepthFormat(DS_Format format) noexcept;
	void SetDepth(bool flag) noexcept;

	D3D12_DEPTH_STENCIL_DESC GetDesc() const noexcept { return m_Desc; }
	DXGI_FORMAT GetFormat() const noexcept { return m_Format; }

private:
	void Default() noexcept;

	D3D12_DEPTH_STENCIL_DESC m_Desc;
	DXGI_FORMAT m_Format;
};

enum class DS_Format :uint8_t
{
	Float = DXGI_FORMAT_D32_FLOAT,
};