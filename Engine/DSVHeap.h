#pragma once

class DSVHeap
{
public:
	~DSVHeap() { Destroy(); }
	void Create();
	void Destroy()
	{
		m_pDepthBuffer.Reset();
		m_pDSVHeap.Reset();
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; };
	const uint32_t GetIncrementSize() const { return m_IncrementSize; }
private:
	ComPtr<ID3D12Resource> m_pDepthBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pDSVHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
	uint32_t m_IncrementSize;
};

