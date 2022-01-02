#pragma once
#include "GpuBuffer.h"

template<typename T>
class IndexBuffer : public GpuBuffer<T>
{
public:
	void Create(uint32_t elementCount)
	{
		constexpr auto strideSize = sizeof(T);

		// Buffer
		GpuBuffer::CreateBuffer(elementCount);

		// View
		m_View.BufferLocation = GetGpuAddress();
		m_View.SizeInBytes = elementCount * strideSize;
		m_View.Format = DXGI_FORMAT_R32_UINT;
	}

	const D3D12_INDEX_BUFFER_VIEW& GetView() const noexcept { return m_View; }

private:
	D3D12_INDEX_BUFFER_VIEW m_View{};
};