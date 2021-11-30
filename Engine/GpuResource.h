#pragma once

class GpuResource
{
public:
	GpuResource() noexcept
		: m_pResource(nullptr)
		, m_GpuVirtualAddress{}
	{
	}

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
};