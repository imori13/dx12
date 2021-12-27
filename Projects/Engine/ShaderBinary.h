#pragma once

class ShaderBinary
{
public:
	void LoadShader(std::wstring_view filePath);
	gsl::not_null<ID3DBlob*> GetBlob() const;
	D3D12_SHADER_BYTECODE GetBytecode() const;
private:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBinaryData;
};