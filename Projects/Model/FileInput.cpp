#include "FileInput.h"
#include "File.h"

#include <fstream>
#include <boost/filesystem.hpp>

namespace
{
	boost::filesystem::wifstream m_ReadingFile;
}

void FileInput::Open(std::wstring_view path)
{
	File::Exists(path);

	m_ReadingFile.clear();
	m_ReadingFile.open(path.data(), std::ios::in);
	ENSURES(m_ReadingFile.is_open(), L"File Open [ %s ]", path.data());
}

bool FileInput::EndOfFile()
{
	return m_ReadingFile.eof();
}

std::wstring FileInput::ReadLine()
{
	EXPECTS(!EndOfFile());

	std::wstring line;
	std::getline(m_ReadingFile, line);

	return line;
}

void FileInput::Close()
{
	m_ReadingFile.close();
	LOGLINE(L"File Closed")
}