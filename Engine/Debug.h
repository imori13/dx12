#pragma once
#include <tchar.h>

namespace Debug
{
#ifdef _DEBUG
	inline void Print(std::string_view msg) noexcept { OutputDebugStringA(msg.data()); }
	inline void Print(std::wstring_view msg) noexcept { OutputDebugString(msg.data()); }
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
#endif

#define LOG_HRESULT(hr) \
	LPTSTR error_text = nullptr; \
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, \
				  nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&error_text, 0, nullptr); \
	Debug::Print(error_text); \

	inline void LogResult(HRESULT flag) noexcept { LOG_HRESULT(flag); }
	inline void LogResult(bool flag) noexcept { flag; }

	constexpr inline bool Check(HRESULT flag) noexcept { return SUCCEEDED(flag); }
	constexpr inline bool Check(bool flag) noexcept { return flag; }

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define FILE_POS_LOG(...) \
			Debug::Printf(__VA_ARGS__); \
			Debug::Print("        ..."); \
			Debug::Print(__FILENAME__); \
			Debug::Print("(" STRINGIFY_BUILTIN(__LINE__)") "); \
			Debug::Print("\n");

	constexpr inline bool HAVESTRING(std::string_view format, ...) noexcept { return format.size() > 0; }
	constexpr inline bool HAVESTRING(std::wstring_view format, ...) noexcept { return format.size() > 0; }
	constexpr inline bool HAVESTRING(void) noexcept { return false; }

#define ASSERT( FLAG, ... ) \
	    if (Debug::Check(FLAG)) \
		{ \
			if(Debug::HAVESTRING(__VA_ARGS__)) \
			{ \
				Debug::Print("SUCCEEDED: "); \
				FILE_POS_LOG(__VA_ARGS__); \
			} \
		} else { \
			Debug::Print("FAILED: "); \
			FILE_POS_LOG(__VA_ARGS__); \
			Debug::LogResult(FLAG); \
			__debugbreak(); \
			std::terminate(); \
		}

	// 事前確認
#define EXPECTS( FLAG, ... ) ASSERT(FLAG, __VA_ARGS__)
	// 事後確認
#define ENSURES( FLAG, ... ) ASSERT(FLAG, __VA_ARGS__)

	// ログ(改行なし)
#define LOG( msg, ... ) \
    Debug::Printf( msg , ##__VA_ARGS__ );
	// ログ(改行あり)
#define LOGLINE( msg, ... ) \
    Debug::Printf( msg "\n", ##__VA_ARGS__ );
};