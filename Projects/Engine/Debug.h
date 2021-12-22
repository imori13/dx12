#pragma once
#include <tchar.h>
#include <system_error>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#pragma warning (disable : 26447)

namespace
{
	constexpr bool SUCCESS_LOG = true;
	constexpr const char* DebugTextFormat = "[%4lu] %-20s : ";
	constexpr const char* DebugTitle = "DEBUG_ERROR";
	constexpr const char* SuccsessHeader = "SUCCEEDED: ";
	constexpr const char* ErrorHeader = "DEBUG_ERROR: ";
}

// Private
namespace
{
	//inline void Print(const std::string_view msg) noexcept { printf("%s", msg.data()); }
	//inline void Print(const std::wstring_view msg) noexcept { wprintf(L"%ws", msg.data()); }
	inline void Print(const std::string_view msg) noexcept { OutputDebugStringA(msg.data()); }
	inline void Print(const std::wstring_view msg) noexcept { OutputDebugStringW(msg.data()); }
	inline void Print(void) noexcept {}

	inline void Printf(const boost::format& format) noexcept { Print(format.str().c_str()); }
	inline void Printf(const boost::wformat& format) noexcept { Print(format.str().c_str()); }

	// çƒãNèàóùÇ≈Boost::formatÇ…ïœä∑ https://theolizer.com/cpp-school2/cpp-school2-30/
	template<typename PARAM_FIRST, typename... PARAMETER>
	inline void Printf(boost::format& format, PARAM_FIRST paramFirst, PARAMETER... parameterT) noexcept
	{ Printf(format % paramFirst, parameterT...); }
	template<typename PARAM_FIRST, typename... PARAMETER>
	inline void Printf(boost::wformat& format, PARAM_FIRST paramFirst, PARAMETER... parameterT) noexcept
	{ Printf(format % paramFirst, parameterT...); }

	template<typename... PARAMETER>
	inline void Printf(std::string_view formatText, PARAMETER... parameterT) noexcept
	{
		boost::format fomat(formatText.data());
		Printf(fomat, parameterT...);
	}
	template<typename... PARAMETER>
	inline void Printf(std::wstring_view formatText, PARAMETER... parameterT) noexcept
	{
		boost::wformat fomat(formatText.data());
		Printf(fomat, parameterT...);
	}
	inline void Printf(void) noexcept {}
}

// Private
namespace
{
	inline std::wstring FileName(std::wstring_view path) noexcept
	{
		std::wstring str = wcsrchr(path.data(), '\\');
		str.erase(str.begin());
		return str;
	}
	inline std::string FileName(std::string_view path) noexcept
	{
		std::string str = strrchr(path.data(), '\\');
		str.erase(str.begin());
		return str;
	}

	inline void LogErrorText(HRESULT hr, uint32_t line, std::string_view file) noexcept
	{
		auto error_text = std::system_category().message(hr);

		std::vector<std::string> splitLine;
		boost::algorithm::split(splitLine, error_text, boost::is_any_of("\n"));
		for(const auto& split : splitLine)
		{
			if(split.empty()) continue;

			Print(ErrorHeader);
			Print(split);
			Print("\n");
		}

		const auto title = boost::format("DEBUG_ERROR -> %s [ line : %lu ]") % FileName(file).c_str() % line;
		MessageBoxA(nullptr, error_text.c_str(), title.str().c_str(), MB_OK | MB_ICONERROR);
	}

	constexpr inline bool Check(bool flag) noexcept { return flag; }
	constexpr inline bool Check(HRESULT flag) noexcept { return SUCCEEDED(flag); }

	template<typename PARAM_FIRST, typename... PARAMETER>
	constexpr inline bool IsEmpty(PARAM_FIRST paramFirst, PARAMETER...) noexcept
	{ return paramFirst == nullptr; }

	template<typename... PAARAMETER>
	constexpr inline bool IsEmpty(PAARAMETER... parameterT) noexcept
	{ return IsEmpty(parameterT...); }

	constexpr inline bool IsEmpty(void) noexcept { return true; }

	inline void LogDirectory(uint32_t line, std::wstring_view file) noexcept
	{ Printf(DebugTextFormat, line, FileName(file).c_str()); }

	inline void LogDirectory(uint32_t line, std::string_view file) noexcept
	{ Printf(DebugTextFormat, line, FileName(file).c_str()); }

	template<typename LINE, typename FILE>
	inline void LogResult(HRESULT flag, LINE lineT, FILE fileT) noexcept { LogErrorText(flag, lineT, fileT); }
	template<typename LINE, typename FILE>
	inline void LogResult(bool, LINE lineT, FILE fileT) noexcept { LogResult(E_ABORT, lineT, fileT); }
}

// Private
namespace
{
	template<typename HEADERTEXT, typename LINE, typename FILE, typename... PARAMETER>
	constexpr inline void DebugText(HEADERTEXT textT, LINE lineT, FILE fileT, PARAMETER... parameterT)
	{
		Print(textT);
		LogDirectory(lineT, fileT);
		Printf(parameterT...);
		Print("\n");
	}

	template<typename LINE, typename FILE, typename... PARAMETER>
	constexpr inline void Succsess(LINE lineT, FILE fileT, PARAMETER... parameterT)
	{
		if(SUCCESS_LOG && !IsEmpty(parameterT...))
			DebugText(SuccsessHeader, lineT, fileT, parameterT...);
	}

	template<typename FLAG, typename LINE, typename FILE, typename... PARAMETER>
	constexpr inline void DebugError(FLAG flagT, LINE lineT, FILE fileT, PARAMETER... parameterT)
	{
		DebugText(ErrorHeader, lineT, fileT, parameterT...);
		LogResult(flagT, lineT, fileT);

		__debugbreak();
	}
}

// Public
#ifdef _DEBUG

#define LOG( msg, ...) Printf(msg, ##__VA_ARGS__)
#define LOGLINE( msg, ...) Printf(msg "\n", ##__VA_ARGS__)

#define ASSERT( FLAG, ... ) \
		if (Check(FLAG)) \
			Succsess( __LINE__, __FILE__, __VA_ARGS__); \
		else \
			DebugError( FLAG, __LINE__, __FILE__, __VA_ARGS__);

#define EXPECTS( FLAG, ...) ASSERT( FLAG, __VA_ARGS__)
#define ENSURES( FLAG, ...) ASSERT( FLAG, __VA_ARGS__)

#else

#define ASSERT( FLAG, ...)
#define EXPECTS( FLAG, ...)
#define ENSURES( FLAG, ...)

#define LOG( msg, ...)
#define LOGLINE( msg, ...)

#endif