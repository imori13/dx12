#pragma once
#include "UploadBuffer.h"

class VertexBuffer : public UploadBuffer
{
public:
	VertexBuffer() noexcept
		:m_BufferView{}
	{
	}

	// ê∂ê¨
	void CreateVertex(size_t bufferSize, size_t strideInBytes)
	{
		UploadBuffer::Create(bufferSize);

		m_BufferView = D3D12_VERTEX_BUFFER_VIEW{ m_GpuVirtualAddress, gsl::narrow<uint32_t>(bufferSize), gsl::narrow<uint32_t>(strideInBytes) };
	}

	D3D12_VERTEX_BUFFER_VIEW GetView() noexcept { return m_BufferView; }
private:
	D3D12_VERTEX_BUFFER_VIEW m_BufferView;
};