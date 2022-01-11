#pragma once

class SS_Binary
{
public:
	void LoadShader(std::wstring_view filePath);

	gsl::not_null<ID3DBlob*> GetBlob() const noexcept { return m_pBinaryData.Get(); }
	D3D12_SHADER_BYTECODE GetBytecode() const
	{
		return D3D12_SHADER_BYTECODE{ m_pBinaryData->GetBufferPointer() ,m_pBinaryData->GetBufferSize() };
	}

private:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBinaryData;
};