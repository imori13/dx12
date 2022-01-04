#pragma once
#include "GpuBuffer.h"
#include "ResourceHeap.h"

template<typename T>
class ConstantBuffer : public GpuBuffer<T>
{
public:
	void Create(Handle_CPU_GPU handle)
	{
		constexpr auto strideSize = sizeof(T);
		constexpr auto elementCount = 1;

		// Buffer
		GpuBuffer::CreateBuffer(elementCount);

		// View
		m_View.BufferLocation = GetGpuAddress();
		m_View.SizeInBytes = strideSize * elementCount;
		Graphics::g_pDevice->CreateConstantBufferView(&m_View, handle.CPU);
	}

	const D3D12_CONSTANT_BUFFER_VIEW_DESC& GetView() const noexcept { return m_View; }

private:
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_View{};
};