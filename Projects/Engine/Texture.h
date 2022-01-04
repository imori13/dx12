#pragma once

class Texture
{
public:
	Texture::Texture()  noexcept
		: m_View{}
	{
	}

	void CreateWIC(const std::wstring_view path);
	void CreateTGA(const std::wstring_view path);

	gsl::not_null<ID3D12Resource*> Get() const noexcept { return m_pResource.Get(); }
	const D3D12_SHADER_RESOURCE_VIEW_DESC& GetView() const noexcept { return m_View; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC m_View;
};

