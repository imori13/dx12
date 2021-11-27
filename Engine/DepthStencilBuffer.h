#pragma once

class DepthStencilBuffer
{
public:
	~DepthStencilBuffer() { Destroy(); }

	void Create(DXGI_FORMAT format, float depthValue, uint8_t stencilValue);
	void Destroy()
	{
		m_pBuffer.Reset();
	}

	ID3D12Resource* Get() { return m_pBuffer.Get(); }
	void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_CpuHandle = handle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() const { return m_CpuHandle; }
	const D3D12_DEPTH_STENCIL_VIEW_DESC GetView() const { return D3D12_DEPTH_STENCIL_VIEW_DESC{ m_pBuffer->GetDesc().Format, D3D12_DSV_DIMENSION_TEXTURE2D }; }
private:
	ComPtr<ID3D12Resource> m_pBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};