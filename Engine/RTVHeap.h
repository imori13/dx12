#pragma once
#include "Display.h"

class RTVHeap
{
public:
	RTVHeap(void)
		: m_pHeap(nullptr)
		, m_IncrementSize(0)
		, m_DescriptorSize(0)
		, m_CpuHandle()
	{

	}
	~RTVHeap() { Destroy(); }

	void Destroy()
	{
		if(m_pHeap != nullptr)
		{
			m_pHeap->Release();
			m_pHeap = nullptr;
		}
	}

	void Create(uint32_t DescriptorSize);

	const D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; };
	const uint32_t GetIncrementSize() const { return m_IncrementSize; }
private:
	ComPtr<ID3D12DescriptorHeap> m_pHeap;
	uint32_t m_IncrementSize;
	uint32_t m_DescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};