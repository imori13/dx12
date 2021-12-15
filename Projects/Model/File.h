#pragma once

struct Path
{
	std::wstring AbsolutePath;	// ��΃p�X
	std::wstring RelativePath;	// ���΃p�X
	std::wstring ParentPath;
	std::wstring FileName;		// �t�@�C���p�X
	std::wstring Extension;		// �g���q
};

namespace File
{
	const bool Exists(std::wstring_view name);
	const Path LoadPath(std::wstring_view name);
}