#pragma once

struct Handle_CPU_GPU
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPU = {};
	D3D12_GPU_DESCRIPTOR_HANDLE GPU = {};
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

	const Handle_CPU_GPU GetHandle()
	{
		const auto rtVal = m_Handle;
		Increment();	// m_Handle_CPU_GPUÇïœçXÇ∑ÇÈ
		return rtVal;
	}
private:
	void Increment();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pHeap;
	uint32_t m_Index;
	uint32_t m_IncrementSize;
	uint32_t m_DescriptorCount;
	Handle_CPU_GPU m_Handle;
};

