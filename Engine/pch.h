#pragma once

#pragma warning (disable : 4324)	// �\���̂̃A���C�������g��ʒm����x�����\��
#pragma warning (disable : 26486)	// TODO : gsl C26486 �x���������ł��Ȃ�
#pragma warning (disable : 26487)	// D3D12_RESOURCE_BARRIR��union���|�C���^�������̌x���𖳎����� �����ł��Ȃ�
#pragma warning (disable : 26490)	// TODO : gsl C26490 ���\�[�XMap���� reinterpret_cast���g�p
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