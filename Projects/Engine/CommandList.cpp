#include "CommandList.h"
#include "GraphicsCore.h"

HRESULT CommandList::Create(uint32_t frameCount)
{
	HRESULT hr{};

	// コマンドアロケータの生成
	{
		m_pCommandAllocators.resize(frameCount);

		for(auto& itr : m_pCommandAllocators)
		{
			hr = Graphics::g_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(itr.GetAddressOf()));

			if(FAILED(hr)) { return hr; }
		}
	}

	// コマンドリストの生成
	{
		hr = Graphics::g_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCommandAllocators.front().Get(),
			nullptr,
			IID_PPV_ARGS(m_pCommandList.GetAddressOf()));

		if(FAILED(hr)) { return hr; }
	}

	hr = m_pCommandList->Close();
	if(FAILED(hr)) { return hr; }

	return NO_ERROR;
}

HRESULT CommandList::Reset(uint32_t frameIndex)
{
	const auto allocator = m_pCommandAllocators.at(frameIndex);

	auto hr = allocator->Reset();
	if(FAILED(hr)) { return hr; }

	hr = m_pCommandList->Reset(allocator.Get(), nullptr);
	if(FAILED(hr)) { return hr; }

	return NO_ERROR;
}

HRESULT CommandList::Close()
{
	const auto hr = m_pCommandList->Close();
	if(FAILED(hr)) { return hr; }

	return NO_ERROR;
}

gsl::not_null<ID3D12GraphicsCommandList*> CommandList::Get() const
{
	return m_pCommandList.Get();
}
