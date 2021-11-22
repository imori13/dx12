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

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

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