#include "SS_Binary.h"
#include "Debug.h"

#include "boost/filesystem.hpp"

void SS_Binary::LoadShader(std::wstring_view filePath)
{
	auto path = boost::filesystem::wpath(filePath.data());
	const auto flag = boost::filesystem::exists(path);
	EXPECTS(flag, "file exists [%s]", path.string());

	auto aa = boost::filesystem::relative(path).wstring();
	D3DReadFileToBlob(aa.c_str(), m_pBinaryData.GetAddressOf());
}