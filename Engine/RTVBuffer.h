#pragma once
#include "GpuResource.h"
#include "GraphicsCore.h"

class RTVBuffer : GpuResource
{
public:
	virtual ~RTVBuffer() { Destroy(); }


	void Destroy()
	{
		m_ColorBuffer.Reset();
	}

	ID3D12Resource* Get() { return m_ColorBuffer.Get(); }
	ID3D12Resource** GetAddressOf() { return m_ColorBuffer.GetAddressOf(); }

	void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_CpuHandle = handle; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return m_CpuHandle; }

private:
	ComPtr<ID3D12Resource> m_ColorBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};

