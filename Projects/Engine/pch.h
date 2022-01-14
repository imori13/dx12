#pragma once

#define NOMINMAX
#define NOHELP
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// vcpkg
#include <gsl/gsl>
#include <DirectXTex.h>
#include <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/WICTextureLoader.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <Eigen/Eigen>

// WindowsSDK
#define DIRECTINPUT_VERSION 0x0800	// DirectXInput version
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <dinput.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <cstdint>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>