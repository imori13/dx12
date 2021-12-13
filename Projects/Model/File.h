#pragma once

namespace File
{
	bool Exists(std::wstring_view path);
}

class FileInput
{
public:
	void Open(std::wstring_view path);
	bool ReadLine(std::wstring& line);
	void Close();
private:
};