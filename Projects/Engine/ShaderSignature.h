#pragma once

class ShaderSignature
{
public:
	void SetSignature(gsl::not_null<ID3DBlob*> shader);
	gsl::not_null<ID3D12RootSignature*> Get() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
};

