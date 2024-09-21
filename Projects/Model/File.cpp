#include "File.h"
#include "Debug.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace File
{
	const bool Exists(std::wstring_view name)
	{
		return boost::filesystem::exists(name.data());
	}

	const Path LoadPath(std::wstring_view name)
	{
		const bool flag = Exists(name);
		ENSURES(flag, L"ファイル検索 [ %s ]", name.data());

		boost::filesystem::wpath boostPath = name.data();

		Path path;
		path.AbsolutePath = boost::filesystem::absolute(name.data()).generic_wstring();
		path.RelativePath = boost::filesystem::relative(name.data()).generic_wstring();
		path.FileName = boostPath.filename().generic_wstring();
		path.Extension = boostPath.filename().extension().generic_wstring();

		std::vector<std::wstring> splitLine;
		boost::algorithm::split(splitLine, name, boost::is_any_of("/"), boost::token_compress_on);
		splitLine.pop_back();	// ファイル名をpopbackしてディレクトリを取得する

		std::wstring parentpath = L"";
		for(const auto& split : splitLine)
		{
			parentpath += split + L"/";
		}
		path.ParentPath = parentpath;

		return path;
	}
}