#pragma once
#include "GpuResource.h"
#include "GraphicsCore.h"

class RenderTargetBuffer : GpuResource
{
public:
	virtual ~RenderTargetBuffer() { Destroy(); }

	void Destroy()
	{
		m_ColorBuffer.Reset();
	}

	ID3D12Resource* Get() { return m_ColorBuffer.Get(); }
	ID3D12Resource** GetAddressOf() { return m_ColorBuffer.GetAddressOf(); }

	void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_CpuHandle = handle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() const { return m_CpuHandle; }

	// swapchain��r8g8b8a8_unorm�܂ł����w��ł��Ȃ����ARTV�ł�r8g8b8a8_unorm_srgb���w��\�Ŏ����I�ɕϊ����Ă����B
	const D3D12_RENDER_TARGET_VIEW_DESC GetView() const { return D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D12_RTV_DIMENSION_TEXTURE2D }; }

private:
	ComPtr<ID3D12Resource> m_ColorBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};

