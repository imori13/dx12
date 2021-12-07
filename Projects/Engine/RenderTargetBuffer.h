#pragma once
#include "GpuResource.h"
#include "GraphicsCore.h"

class RenderTargetBuffer : GpuResource
{
public:
	RenderTargetBuffer() noexcept
		: m_ColorBuffer(nullptr)
		, m_CpuHandle{}
	{}

	ID3D12Resource* Get() noexcept { return m_ColorBuffer.Get(); }
	ID3D12Resource** GetAddressOf() noexcept { return m_ColorBuffer.GetAddressOf(); }

	void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) noexcept { m_CpuHandle = handle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() const noexcept { return m_CpuHandle; }

	// swapchain��r8g8b8a8_unorm�܂ł����w��ł��Ȃ����ARTV�ł�r8g8b8a8_unorm_srgb���w��\�Ŏ����I�ɕϊ�����swapchain�ɓn���Ă����B
	const D3D12_RENDER_TARGET_VIEW_DESC GetView() const noexcept { return D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RTV_DIMENSION_TEXTURE2D }; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_ColorBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};

