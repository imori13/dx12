#pragma once

namespace Debug
{
#ifdef _DEBUG
	inline void Print(std::string_view msg) noexcept { OutputDebugStringA(msg.data()); }
	inline void Print(std::wstring_view msg) noexcept { OutputDebugString(msg.data()); }
#endif

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

	inline void PrintSubMessage(std::string_view format, ...) noexcept
	{
		Print("--> ");
		char buffer[256];
		const auto bufferSpan = gsl::make_span(buffer);
		va_list ap;
		va_start(ap, format);
		vsprintf_s(bufferSpan.data(), 256, format.data(), ap);
		ap = nullptr;

		Print(bufferSpan.data());
		Print("\n");
	}
	inline void PrintSubMessage(std::wstring_view format, ...) noexcept
	{
		Print("--> ");
		wchar_t buffer[256];
		const auto bufferSpan = gsl::make_span(buffer);
		va_list ap;
		va_start(ap, format);
		vswprintf(bufferSpan.data(), 256, format.data(), ap);
		ap = nullptr;
		//va_end(ap);
		Print(bufferSpan.data());
		Print("\n");
	}
	inline void PrintSubMessage(void) noexcept
	{
	}

#ifdef _DEBUG

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

	// 事前確認
#define EXPECTS( FLAG, ... ) \
	    if (!(bool)(FLAG)) { \
	        Debug::Print("\nEXPECTS() failed in "); \
	        Debug::Print(__FILENAME__); \
	        Debug::Print("(" STRINGIFY_BUILTIN(__LINE__)")\n"); \
	        Debug::PrintSubMessage("FALSE IS \'" #FLAG "\'"); \
	        Debug::PrintSubMessage(__VA_ARGS__); \
	        Debug::Print("\n"); \
	        __debugbreak(); \
	    }

	// 事後確認
#define ENSURES( FLAG, ... ) \
		if(!(bool)(FLAG)) { \
			Debug::Print("\nENSURES() failed in "); \
			Debug::Print(__FILENAME__); \
			Debug::Print("(" STRINGIFY_BUILTIN(__LINE__)")\n"); \
			Debug::PrintSubMessage("FALSE IS \'" #FLAG "\'"); \
			Debug::PrintSubMessage(__VA_ARGS__); \
			Debug::Print("\n"); \
			__debugbreak(); \
		}

	// ログ(改行なし)
#define LOG( msg, ... ) \
    Debug::Printf( msg , ##__VA_ARGS__ );
	// ログ(改行あり)
#define LOGLINE( msg, ... ) \
    Debug::Printf( msg "\n", ##__VA_ARGS__ );

#define BREAK( ... ) ERROR( __VA_ARGS__ ) __debugbreak();

#define ERROR( ... ) \
        Debug::Print("\nERROR reported in "); \
        Debug::Print(__FILENAME__); \
        Debug::Print("(" STRINGIFY_BUILTIN(__LINE__)")\n"); \
        Debug::PrintSubMessage(__VA_ARGS__); \
        Debug::Print("\n");
#endif
};