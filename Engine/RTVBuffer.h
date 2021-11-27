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
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() const { return m_CpuHandle; }
	const D3D12_RENDER_TARGET_VIEW_DESC GetView() const { return D3D12_RENDER_TARGET_VIEW_DESC{ m_ColorBuffer.Get()->GetDesc().Format ,D3D12_RTV_DIMENSION_TEXTURE2D }; }

private:
	ComPtr<ID3D12Resource> m_ColorBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};

