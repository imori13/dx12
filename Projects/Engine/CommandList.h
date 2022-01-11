#pragma once

class CommandList
{
public:
	HRESULT Create(uint32_t frameCount);
	HRESULT Reset(uint32_t frameIndex);
	HRESULT Close();

	gsl::not_null<ID3D12GraphicsCommandList*> Get() const;
private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_pCommandAllocators;
};

