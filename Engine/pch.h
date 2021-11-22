#pragma once

#define NOMINMAX
#define NOHELP
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