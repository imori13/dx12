#pragma once

#include "pch.h"

namespace Utility
{
#ifdef _CONSOLE
	inline void Print(const char* msg) { printf("%s", msg); }
	inline void Print(const wchar_t* msg) { wprintf(L"%ws", msg); }
#else
	inline void Print(const char* msg) { OutputDebugStringA(msg); }
	inline void Print(const wchar_t* msg) { OutputDebugString(msg); }
#endif

	inline void Printf(const char* format, ...)
	{
		char buffer[256];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
	}

	inline void Printf(const wchar_t* format, ...)
	{
		wchar_t buffer[256];
		va_list ap;
		va_start(ap, format);
		vswprintf(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
	}

#ifndef RELEASE
	inline void PrintSubMessage(const char* format, ...)
	{
		Print("--> ");
		char buffer[256];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
		Print("\n");
	}
	inline void PrintSubMessage(const wchar_t* format, ...)
	{
		Print("--> ");
		wchar_t buffer[256];
		va_list ap;
		va_start(ap, format);
		vswprintf(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
		Print("\n");
	}
	inline void PrintSubMessage(void)
	{
	}
#endif

	std::wstring UTF8ToWideString(const std::string& str);
	std::string WideStringToUTF8(const std::wstring& wstr);
	std::string ToLower(const std::string& str);
	std::wstring ToLower(const std::wstring& str);
	std::string GetBasePath(const std::string& str);
	std::wstring GetBasePath(const std::wstring& str);
	std::string RemoveBasePath(const std::string& str);
	std::wstring RemoveBasePath(const std::wstring& str);
	std::string GetFileExtension(const std::string& str);
	std::wstring GetFileExtension(const std::wstring& str);
	std::string RemoveExtension(const std::string& str);
	std::wstring RemoveExtension(const std::wstring& str);


} // namespace Utility

#ifdef RELEASE

	#define ASSERT( isTrue, ... ) (void)(isTrue)
	#define ERROR( msg, ... )
	#define DEBUGPRINT( msg, ... ) do {} while(0)
	#define ASSERT_SUCCEEDED( hr, ... ) (void)(hr)

#else   // !RELEASE

	#define STRINGIFY(x) #x
	#define STRINGIFY_BUILTIN(x) STRINGIFY(x)
	#define ASSERT( isFalse, ... ) \
        if (!(bool)(isFalse)) { \
            Utility::Print("\nAssertion failed in " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
            Utility::PrintSubMessage("\'" #isFalse "\' is false"); \
            Utility::PrintSubMessage(__VA_ARGS__); \
            Utility::Print("\n"); \
            __debugbreak(); \
        }

	#define ASSERT_SUCCEEDED( hr, ... ) \
        if (FAILED(hr)) { \
            Utility::Print("\nHRESULT failed in " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
            Utility::PrintSubMessage("hr = 0x%08X", hr); \
            Utility::PrintSubMessage(__VA_ARGS__); \
            Utility::Print("\n"); \
            __debugbreak(); \
        }

	#define ERROR( ... ) \
        Utility::Print("\nError reported in " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
        Utility::PrintSubMessage(__VA_ARGS__); \
        Utility::Print("\n");

	#define DEBUGPRINT( msg, ... ) \
    Utility::Printf( msg "\n", ##__VA_ARGS__ );

#endif

#define BreakIfFailed( hr ) if (FAILED(hr)) __debugbreak()