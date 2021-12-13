#include "File.h"

#include <boost/filesystem.hpp>

namespace
{
	boost::filesystem::wifstream m_ReadingFile;
}

namespace File
{
	bool Exists(std::wstring_view path)
	{
		return boost::filesystem::exists(path.data());
	}
}

void FileInput::Open(std::wstring_view path)
{
	EXPECTS(File::Exists(path), L"�t�@�C�����݃`�F�b�N [%s]", path.data());

	m_ReadingFile.open(path.data(), std::ios::in);
	ENSURES(m_ReadingFile.is_open(), L"�t�@�C���I�[�v�� [ %s ]", path.data());
}

bool FileInput::ReadLine(std::wstring& line)
{
	// EOF��
	if(m_ReadingFile.eof())
	{ return false; }

	std::getline(m_ReadingFile, line);

	return true;
}

void FileInput::Close()
{
	m_ReadingFile.close();
}