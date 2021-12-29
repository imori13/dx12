#pragma once

namespace
{
	constexpr uint8_t MAX_RTV_COUNT = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
}

enum class RTV_Format : uint8_t;
enum class RTV_Blend : uint8_t;

class PS_RenderTarget
{
public:
	PS_RenderTarget() noexcept
	{
		Default();
	}

	void SetRtvFormat(uint8_t rtvIndex, RTV_Format format);
	void SetRtvBlend(uint8_t rtvIndex, RTV_Blend blend);
	void SetRtvNum(uint8_t rtvNum) noexcept;

	std::array<DXGI_FORMAT, MAX_RTV_COUNT> GetRtvFormat() const noexcept { return m_RtvFormat; }
	D3D12_BLEND_DESC GetBlendDesc() const noexcept { return m_BlendDesc; }
	uint8_t GetRtvNum() const noexcept { return m_RtvNum; }

private:
	void Default() noexcept;

	std::array<DXGI_FORMAT, MAX_RTV_COUNT> m_RtvFormat;
	D3D12_BLEND_DESC m_BlendDesc;
	uint8_t m_RtvNum;
};

enum class RTV_Format : uint8_t
{
	Float4 = DXGI_FORMAT_R8G8B8A8_UNORM,
};

enum class RTV_Blend : uint8_t
{
	None,
};