#pragma once
#include "GpuResource.h"
#include "GraphicsCore.h"

class RTVBuffer : GpuResource
{
public:
	RTVBuffer()
		: m_ColorBuffer(nullptr)
	{
	}
	~RTVBuffer() { Destroy(); }

	void Destroy()
	{
		m_ColorBuffer.Reset();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
	ID3D12Resource* Get() { return m_ColorBuffer.Get(); }
	ID3D12Resource** GetAddressOf() { return m_ColorBuffer.GetAddressOf(); }
private:
	ComPtr<ID3D12Resource> m_ColorBuffer;
};

