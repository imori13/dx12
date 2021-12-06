#pragma once

#pragma warning (disable : 4324)	// �\���̂̃A���C�������g��ʒm����x�����\��
#pragma warning (disable : 26486)	// TODO : gsl C26486 �x���������ł��Ȃ�
#pragma warning (disable : 26487)	// D3D12_RESOURCE_BARRIR��union���|�C���^�������̌x���𖳎����� �����ł��Ȃ�
#pragma warning (disable : 26490)	// TODO : gsl C26490 ���\�[�XMap���� reinterpret_cast���g�p


#define NOMINMAX
#define NOHELP
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK12.lib")

#include <GSL/gsl>
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/WICTextureLoader.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <DirectXMath.h>

#include "Debug.h"