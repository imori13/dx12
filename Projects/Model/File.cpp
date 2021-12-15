#include "File.h"

#include <boost/filesystem.hpp>

namespace
{
	boost::filesystem::wifstream m_ReadingFile;
}

namespace File
{

	const bool Exists(std::wstring_view name)
	{
		const bool flag = boost::filesystem::exists(name.data());
		ENSURES(flag == true, L"ÉtÉ@ÉCÉãåüçı [ %s ]", name.data());
		return flag;
	}

	const Path LoadPath(std::wstring_view name)
	{
		Exists(name);

		boost::filesystem::wpath boostPath = name.data();

		Path path;
		path.AbsolutePath = boost::filesystem::absolute(name.data()).generic_wstring();
		path.RelativePath = boost::filesystem::relative(name.data()).generic_wstring();
		path.FileName = boostPath.filename().generic_wstring();
		path.Extension = boostPath.filename().extension().generic_wstring();

		return path;
	}

	void FileInput::Open(std::wstring_view path)
	{
		File::Exists(path);

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
}