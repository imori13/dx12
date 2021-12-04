#include "UploadBuffer.h"
#include "GraphicsCore.h"


void UploadBuffer::Create(size_t bufferSize, size_t strideSize)
{
	m_BufferSize = bufferSize;
	m_StrideSize = strideSize;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES HeapProps;
	HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProps.CreationNodeMask = 1;
	HeapProps.VisibleNodeMask = 1;

	// リソース設定
	D3D12_RESOURCE_DESC ResourceDesc = {};
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = m_BufferSize;
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// 生成
	const auto hr = Graphics::g_pDevice->CreateCommittedResource(
		&HeapProps,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pResource));
	ENSURES(hr, "Buffer生成");

	// アドレスを取得
	m_GpuVirtualAddress = m_pResource->GetGPUVirtualAddress();
}

void* UploadBuffer::Map()
{
	void* ptr = nullptr;
	m_pResource->Map(0, nullptr, &ptr);
	return ptr;
}

void UploadBuffer::UnMap()
{
	m_pResource->Unmap(0, nullptr);
}