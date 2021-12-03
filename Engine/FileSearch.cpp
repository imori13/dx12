#include "FileSearch.h"

bool SearchFilePath(const wchar_t* filename, std::wstring& retFilepath) noexcept
{
	// 入力がなければ終了
	if(filename == nullptr)
	{
		return false;
	}

	// 入力がなければ終了
	if(wcscmp(filename, L" ") == 0 || wcscmp(filename, L"") == 0)
	{
		return false;
	}

	constexpr size_t size = 520;
	std::array<wchar_t, size> exePath = {};
	GetModuleFileNameW(nullptr, exePath.data(), gsl::narrow<DWORD>(exePath.max_size()));
	PathRemoveFileSpecW(exePath.data());

	std::array<wchar_t, size> dstPath = {};

	wcscpy_s(dstPath.data(), dstPath.max_size(), filename);
	if(PathFileExistsW(dstPath.data()) == TRUE)
	{
		retFilepath = dstPath.data();
		return true;
	}

	swprintf_s(dstPath.data(), dstPath.max_size(), L"..\\%s", filename);
	if(PathFileExistsW(dstPath.data()) == TRUE)
	{
		retFilepath = dstPath.data();
		return true;
	}

	swprintf_s(dstPath.data(), dstPath.max_size(), L"..\\..\\%s", filename);
	if(PathFileExistsW(dstPath.data()) == TRUE)
	{
		retFilepath = dstPath.data();
		return true;
	}

	swprintf_s(dstPath.data(), dstPath.max_size(), L"%s\\%s", exePath.data(), filename);
	if(PathFileExistsW(dstPath.data()) == TRUE)
	{
		retFilepath = dstPath.data();
		return true;
	}

	swprintf_s(dstPath.data(), dstPath.max_size(), L"%s\\..\\%s", exePath.data(), filename);
	if(PathFileExistsW(dstPath.data()) == TRUE)
	{
		retFilepath = dstPath.data();
		return true;
	}

	swprintf_s(dstPath.data(), dstPath.max_size(), L"%s\\..\\..\\%s", exePath.data(), filename);
	if(PathFileExistsW(dstPath.data()) == TRUE)
	{
		retFilepath = dstPath.data();
		return true;
	}

	return false;
}
