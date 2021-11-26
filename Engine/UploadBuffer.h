#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	virtual ~UploadBuffer() { Destroy(); }

	void Create(size_t bufferSize);

	HRESULT Map(void** ptr);
	void UnMap();

	size_t GetBufferSize() const { return m_BufferSize; }

	D3D12_VERTEX_BUFFER_VIEW CreateVertexBuffer(uint32_t sizeInByte, uint32_t strideInBytes)
	{
		D3D12_VERTEX_BUFFER_VIEW m_VBV;
		m_VBV.BufferLocation = m_GpuVirtualAddress;
		m_VBV.SizeInBytes = sizeInByte;
		m_VBV.StrideInBytes = strideInBytes;

		return m_VBV;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() { return m_GpuVirtualAddress; }

	void SetCBV(ID3D12GraphicsCommandList* cmdList)
	{
		cmdList->SetGraphicsRootConstantBufferView(0, m_GpuVirtualAddress);
	}
protected:
	size_t m_BufferSize;
};

