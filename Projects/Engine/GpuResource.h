#pragma once
#include "Debug.h"
#include "GraphicsCore.h"

namespace
{
	// HeapÝ’è
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

	// ReousrceDescÝ’è
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

	// ConstantBufferViewÝ’è
	constexpr inline D3D12_CONSTANT_BUFFER_VIEW_DESC GetCBufferView(
		D3D12_GPU_VIRTUAL_ADDRESS location,
		uint32_t sizeinbyte) noexcept
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc = {};
		ViewDesc.BufferLocation = location;
		ViewDesc.SizeInBytes = sizeinbyte;
		return ViewDesc;
	}
}

class GpuResource
{
public:
	GpuResource() noexcept
		: m_pResource(nullptr)
		, m_GpuAddress(0)
	{

	}

protected:
	void CreateBuffer(uint32_t bufferSize)
	{
		const auto heap = GetDefaultHeapProp();
		const auto resDesc = GetDefaultResourceDesc(bufferSize);

		// Create
		const auto hr = Graphics::g_pDevice->CreateCommittedResource(
			&heap,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pResource));
		ENSURES(hr, "Buffer¶¬");

		// Address
		m_GpuAddress = m_pResource->GetGPUVirtualAddress();
	}

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuAddress;
};