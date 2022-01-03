#pragma once
#include "Debug.h"
#include "GraphicsCore.h"

namespace
{
	// Heapê›íË
	constexpr inline D3D12_HEAP_PROPERTIES GetDefaultHeapProp() noexcept
	{
		D3D12_HEAP_PROPERTIES HeapProps = {};
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		return HeapProps;
	}

	// ReousrceDescê›íË
	constexpr inline D3D12_RESOURCE_DESC GetDefaultResourceDesc(uint64_t bufferSize) noexcept
	{
		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = bufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		return ResourceDesc;
	}
}

template<typename T>
class GpuBuffer
{
public:
	GpuBuffer() noexcept
		: m_pResource(nullptr)
		, m_GpuAddress(0)
		, m_Datas{}
	{

	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const noexcept { return m_GpuAddress; }

	T& data() noexcept { return *m_Datas.front(); }
	T& at(uint32_t i) noexcept { return *m_Datas.at(i); }
	size_t size() const noexcept { return m_Datas.size(); }

	void MemCopy(const gsl::span<const T> span) noexcept
	{
		const auto size = span.size_bytes();
		memcpy_s(&data(), size, span.data(), size);
	}

	void MemCopy(const T& data) noexcept
	{
		constexpr auto size = sizeof(T);
		memcpy_s(&this->data(), size, &data, size);
	}

protected:
	void CreateBuffer(uint32_t elementCount)
	{
		constexpr auto strideSize = sizeof(T);
		const auto heap = GetDefaultHeapProp();
		const auto resDesc = GetDefaultResourceDesc(elementCount * strideSize);

		// create buffer
		const auto hr = Graphics::g_pDevice->CreateCommittedResource(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pResource));
		ENSURES(hr, "Bufferê∂ê¨");
		// set address
		m_GpuAddress = m_pResource->GetGPUVirtualAddress();

		// map
		uint8_t* ptr = nullptr;
		m_pResource->Map(0, nullptr, &static_cast<void*>(ptr));

		// collect in vector
		m_Datas.resize(elementCount);
		for(auto i = 0; i < m_Datas.size(); ++i)
		{
			m_Datas.at(i) = reinterpret_cast<T*>(ptr);
			ptr += strideSize;
		}
	}

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;

private:
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuAddress;
	std::vector<T*> m_Datas;
};