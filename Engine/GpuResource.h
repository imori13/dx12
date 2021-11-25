#pragma once

class GpuResource
{
public:
	GpuResource()
		: m_pResource(nullptr)
		, m_GpuVirtualAddress(ADDRESS_NULL)
		, m_UsageState(D3D12_RESOURCE_STATE_COMMON)
		, m_TransitioningState((D3D12_RESOURCE_STATES)-1)
	{
	}

	~GpuResource()
	{
		Destroy();
	}

	void Destroy()
	{
		m_pResource = nullptr;
		m_GpuVirtualAddress = ADDRESS_NULL;
	}

protected:
	ComPtr<ID3D12Resource> m_pResource;
	D3D12_RESOURCE_STATES m_UsageState;
	D3D12_RESOURCE_STATES m_TransitioningState;
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
};