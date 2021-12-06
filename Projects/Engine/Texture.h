#pragma once

#include "GpuResource.h"
#include "ResourceHeap.h"

#include "GraphicsCore.h"

class Texture : public GpuResource
{
public:
	Texture() noexcept
		: m_HandleCPU{}
		, m_HandleGPU{}
	{
	}
	void Create(const std::wstring& path);
	void CreateWIC(const std::wstring& path);

	const void SetHeap(ResourceHeap& heap, const uint32_t index)
	{
		m_HandleCPU = heap.GetCPUHandle(index);
		m_HandleGPU = heap.GetGPUHandle(index);

		Graphics::g_pDevice->CreateShaderResourceView(m_pResource.Get(), &m_View, m_HandleCPU);
	}
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() noexcept { return m_HandleGPU; }
private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE m_HandleGPU;
	D3D12_SHADER_RESOURCE_VIEW_DESC m_View;
};

