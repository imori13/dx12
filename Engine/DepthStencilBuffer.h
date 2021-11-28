#pragma once

class DepthStencilBuffer
{
public:
	DepthStencilBuffer() noexcept
		: m_pBuffer(nullptr)
		, m_CpuHandle{}
	{}

	void Create(DXGI_FORMAT format, float depthValue, uint8_t stencilValue);

	ID3D12Resource* Get() noexcept { return m_pBuffer.Get(); }
	void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) noexcept { m_CpuHandle = handle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() const noexcept { return m_CpuHandle; }
	const D3D12_DEPTH_STENCIL_VIEW_DESC GetView() const { return D3D12_DEPTH_STENCIL_VIEW_DESC{ m_pBuffer->GetDesc().Format, D3D12_DSV_DIMENSION_TEXTURE2D }; }
private:
	ComPtr<ID3D12Resource> m_pBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};