#pragma once

struct Handle_CPU_GPU
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPU = {};
	D3D12_GPU_DESCRIPTOR_HANDLE GPU = {};
	uint32_t IncrementSize = 0;
};

class ResourceHeap
{
public:
	ResourceHeap() noexcept
		: m_pHeap(nullptr)
		, m_Index(0)
		, m_DescriptorCount(0)
		, m_IncrementSize(0)
		, m_Handle{}
	{}

	// ê∂ê¨
	void Create(uint32_t bufferCount, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag);

	gsl::not_null<ID3D12DescriptorHeap*> Get() { return m_pHeap.Get(); }
	gsl::not_null<ID3D12DescriptorHeap**> GetAddress() { return m_pHeap.GetAddressOf(); }

	void InitializeIndex() noexcept { m_Index = 0; }
	const Handle_CPU_GPU GetNextHandle()
	{
		EXPECTS(m_Index >= 0 && m_Index < m_DescriptorCount);

		Handle_CPU_GPU rtVal = m_Handle;

		rtVal.CPU.ptr += gsl::narrow<uint64_t>(m_IncrementSize * m_Index);
		rtVal.GPU.ptr += gsl::narrow<uint64_t>(m_IncrementSize * m_Index);
		rtVal.IncrementSize = m_IncrementSize;

		++m_Index;

		return rtVal;
	}
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pHeap;
	uint32_t m_Index;
	uint32_t m_IncrementSize;
	uint32_t m_DescriptorCount;
	Handle_CPU_GPU m_Handle;
};

