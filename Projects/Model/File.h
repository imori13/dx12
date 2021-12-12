#pragma once

namespace File
{
	void Open(std::wstring_view path);
	bool ReadLine(std::wstring_view line);
	void Close();
}