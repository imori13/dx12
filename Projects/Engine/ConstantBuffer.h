#pragma once
#include "GpuResource.h"

template<typename T>
class ConstantBuffer : GpuResource
{
public:
	ConstantBuffer() noexcept
		: m_Constant(nullptr)
		, m_ConstantView{}
	{}

	void Create(Handle_CPU_GPU handle);

	D3D12_GPU_VIRTUAL_ADDRESS GetBufferLocation() const noexcept { return m_GpuAddress; }
	gsl::not_null<T*> GetData() const noexcept { return m_Constant; }

private:
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantView;
	T* m_Constant;

};

template<typename T>
void ConstantBuffer<T>::Create(Handle_CPU_GPU handle)
{
	constexpr auto bufferSize = sizeof(T);

	// Buffer
	CreateBuffer(bufferSize);

	// View
	m_ConstantView = GetCBufferView(m_GpuAddress, bufferSize);
	Graphics::g_pDevice->CreateConstantBufferView(&m_ConstantView, handle.CPU);

	// Map
	m_pResource->Map(0, nullptr, &static_cast<void*>(m_Constant));
}