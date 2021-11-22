#pragma once

// windows.hのmin,max関数を使用しない。c++標準のを使う
#define NOMINMAX

// WinHelp is deprecated
#define NOHELP

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

#include "Utility.h"