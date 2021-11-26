#pragma once
#include "Display.h"
#include "RTVHeap.h"
#include "Command.h"

class RTVHeap;
class Command;

namespace Graphics
{
	extern ComPtr<ID3D12Device> g_pDevice;
	extern Command g_Command;

	bool Initialize();
	void Terminate();
}