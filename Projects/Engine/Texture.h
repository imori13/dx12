#pragma once

#include "GpuResource.h"
#include "ResourceHeap.h"

#include "GraphicsCore.h"

class Texture : public GpuResource
{
public:
	Texture::Texture()  noexcept
		: m_View{}
	{
	}

	void CreateWIC(const std::wstring_view path);

	gsl::not_null<ID3D12Resource*> Get() const noexcept { return m_pResource.Get(); }
	D3D12_SHADER_RESOURCE_VIEW_DESC GetView() const noexcept { return m_View; }

private:
	D3D12_SHADER_RESOURCE_VIEW_DESC m_View;
};

