#pragma once

class PS_Rasterizer
{
public:
	PS_Rasterizer() noexcept
	{
		Default();
	}

	void SetSolidMode(bool flag) noexcept;
	void SetCulling(bool flag) noexcept;
	void SetMultiSample(bool flag, uint32_t count, uint32_t quality) noexcept;

	D3D12_RASTERIZER_DESC GetRasterizerDesc()const noexcept { return m_RasterizerDesc; }
	DXGI_SAMPLE_DESC GetSampleDesc()const noexcept { return m_SampleDesc; }
	uint32_t GetSampleMask()const noexcept { return m_SampleMask; }

private:
	void Default() noexcept;

	D3D12_RASTERIZER_DESC m_RasterizerDesc;
	DXGI_SAMPLE_DESC m_SampleDesc;
	uint32_t m_SampleMask;
};

