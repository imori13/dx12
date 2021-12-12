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

	void File::Open(std::wstring_view path)
	{
		EXPECTS(Exists(path), L"ファイルチェック");

		m_ReadingFile.open(path.data(), std::ios::in);

		std::wstring reading_line_buffer;
		std::getline(m_ReadingFile, reading_line_buffer);
	}

	bool File::ReadLine(std::wstring_view line)
	{
		//std::wstring reading_line_buffer;
		///*boost::filesystem::lin*/
		//std::getline(m_ReadingFile, line.data());
		//std::getline(m_ReadingFile, reading_line_buffer);

		return false;
	}

	void File::Close()
	{
	}
}