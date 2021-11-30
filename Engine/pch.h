#pragma once

#pragma warning (disable : 4324)	// 構造体のアラインメントを通知する警告を非表示
#pragma warning (disable : 26486)	// TODO : gsl C26486 警告を解決できない
#pragma warning (disable : 26487)	// D3D12_RESOURCE_BARRIRのunion内ポインタが原因の警告を無視する 解決できない
#pragma warning (disable : 26490)	// TODO : gsl C26490 リソースMap時に reinterpret_castを使用
//#pragma warning (disable : 26481)

#define NOMINMAX
#define NOHELP
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#include <gsl/gsl>
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>

#include "Debug.h"