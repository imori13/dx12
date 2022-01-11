#pragma once

class SS_RootSignature
{
public:
	void SetSignature(gsl::not_null<ID3DBlob*> shader);

	gsl::not_null<ID3DBlob*> GetBlob() const noexcept { return m_pRootSignatureBlob.Get(); }
	D3D12_SHADER_BYTECODE GetBytecode() const
	{
		return D3D12_SHADER_BYTECODE{ m_pRootSignatureBlob->GetBufferPointer() ,m_pRootSignatureBlob->GetBufferSize() };
	}

private:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pRootSignatureBlob;
};

