#pragma once

class FileInput
{
public:
	void Open(std::wstring_view path);
	bool EndOfFile();
	std::wstring ReadLine();
	void Close();
private:
};