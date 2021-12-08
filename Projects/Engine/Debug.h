#pragma once
#include <tchar.h>
#include <GSL\gsl>

namespace Debug
{
	constexpr inline bool Check(HRESULT flag) noexcept(false) { return SUCCEEDED(flag); }
	constexpr inline bool Check(bool flag) noexcept(false) { return flag; }

#ifdef _DEBUG
	//inline void Print(const std::string_view msg) noexcept { printf("%s", msg.data()); }
	//inline void Print(const std::wstring_view msg) noexcept { wprintf(L"%ws", msg.data()); }
	inline void Print(const std::string_view msg) noexcept { OutputDebugStringA(msg.data()); }
	inline void Print(const std::wstring_view msg) noexcept { OutputDebugStringW(msg.data()); }
	inline void Print(void) noexcept {}

	inline void Printf(std::string_view format, ...) noexcept
	{
		char buffer[256];
		const auto bufferSpan = gsl::make_span(buffer);
		va_list ap;
		va_start(ap, format);
		vsprintf_s(bufferSpan.data(), 256, format.data(), ap);
		ap = nullptr;
		Print(bufferSpan.data());
	}

	inline void Printf(std::wstring_view format, ...) noexcept
	{
		wchar_t buffer[256];
		const auto bufferSpan = gsl::make_span(buffer);
		va_list ap;
		va_start(ap, format);
		vswprintf(bufferSpan.data(), 256, format.data(), ap);
		ap = nullptr;
		Print(bufferSpan.data());
	}
	inline void Printf(void) noexcept {}

#define LOG_HRESULT(hr) \
	LPTSTR error_text = nullptr; \
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, \
				  nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&error_text, 0, nullptr); \
	Debug::Print(error_text); \

	inline void LogResult(HRESULT flag) noexcept { LOG_HRESULT(flag); }
	inline void LogResult(bool flag) noexcept { flag; }

	inline char const* GetFileName(std::string_view path) noexcept
	{
#pragma warning( push )
#pragma warning (disable : 26481)
		const auto str = strrchr(path.data(), '\\');
		return (str != nullptr) ? (str + 1) : (str);
#pragma warning( pop )
	}

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)
#define FILE_POS_LOG(...) \
		LOG(_T("[%2s]%-20s : "), STRINGIFY_BUILTIN(__LINE__), Debug::GetFileName(__FILE__)); \
		Debug::Printf(__VA_ARGS__); \
		Debug::Print(_T("\n"));

	constexpr inline bool HAVESTRING(std::string_view format, ...) noexcept { return format.size() > 0; }
	constexpr inline bool HAVESTRING(std::wstring_view format, ...) noexcept { return format.size() > 0; }
	constexpr inline bool HAVESTRING(void) noexcept { return false; }

	// ログ(改行なし)
#define LOG( msg, ... ) \
    Debug::Printf( msg , ##__VA_ARGS__ );
	// ログ(改行あり)
#define LOGLINE( msg, ... ) \
    Debug::Printf(_T("%s\n"), msg, ##__VA_ARGS__ );

#define ASSERT( FLAG, ... ) \
	if (Debug::Check(FLAG)) \
		{ \
			if(Debug::HAVESTRING(__VA_ARGS__)) \
			{ \
				Debug::Print(_T("SUCCEEDED: ")); \
				FILE_POS_LOG(__VA_ARGS__); \
			} \
		} else { \
			Debug::Print(_T("FAILED: ")); \
			FILE_POS_LOG(__VA_ARGS__); \
			Debug::LogResult(FLAG); \
			__debugbreak(); \
			std::terminate(); \
		}

#elif _RELEASE

	// 事前確認
#define ASSERT(FLAG, ... ) (FLAG) ? (static_cast<void>(0)) : (std::terminate())
#define LOG( msg, ... ) {}
#define LOGLINE( msg, ... ) {}

#endif

	// 事前確認
#define EXPECTS( FLAG, ... ) ASSERT(Debug::Check(FLAG),__VA_ARGS__)
	// 事後確認
#define ENSURES( FLAG, ... ) ASSERT(Debug::Check(FLAG),__VA_ARGS__)
};