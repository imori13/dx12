#pragma once

struct Path
{
	std::wstring AbsolutePath;	// 絶対パス
	std::wstring RelativePath;	// 相対パス
	std::wstring ParentPath;
	std::wstring FileName;		// ファイルパス
	std::wstring Extension;		// 拡張子
};

namespace File
{
	const bool Exists(std::wstring_view name);
	const Path LoadPath(std::wstring_view name);
}