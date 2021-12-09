#pragma once

#include "GpuResource.h"
#include "ResourceHeap.h"

#include "GraphicsCore.h"

class Texture : public GpuResource
{
public:
	Texture::Texture()  noexcept
		: m_HandleCPU{}
		, m_HandleGPU{}
		, m_View{}
	{
	}

	void CreateWIC(const std::wstring_view path);

	gsl::not_null<ID3D12Resource*> Get() noexcept { return m_pResource.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE m_HandleGPU;
	D3D12_SHADER_RESOURCE_VIEW_DESC m_View;
};

