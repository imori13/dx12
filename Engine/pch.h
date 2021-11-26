#pragma once

#define NOMINMAX
#define NOHELP
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#define ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#pragma comment(lib,"d3D12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

template<typename T>using ComPtr = Microsoft::WRL::ComPtr<T>;

#include <cstdint>
#include <vector>
#include <memory>
#include <string>