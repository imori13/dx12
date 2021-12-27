#include "ShaderBinary.h"
#include "Debug.h"

#include "boost/filesystem.hpp"

void ShaderBinary::LoadShader(std::wstring_view filePath)
{
	auto path = boost::filesystem::wpath(filePath.data());
	const auto flag = boost::filesystem::exists(path);
	EXPECTS(flag, "file exists [%s]", path.string());

	auto aa = boost::filesystem::relative(path).wstring();
	D3DReadFileToBlob(aa.c_str(), m_pBinaryData.GetAddressOf());
}

gsl::not_null<ID3DBlob*> ShaderBinary::GetBlob() const
{
	return m_pBinaryData.Get();
}

D3D12_SHADER_BYTECODE ShaderBinary::GetBytecode() const
{
	D3D12_SHADER_BYTECODE byteCode;
	byteCode.pShaderBytecode = m_pBinaryData->GetBufferPointer();
	byteCode.BytecodeLength = m_pBinaryData->GetBufferSize();

	return byteCode;
}
