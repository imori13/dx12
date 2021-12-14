#include "File.h"

#include <boost/filesystem.hpp>

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
}