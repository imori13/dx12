#include "Display.h"

#define SWAP_CHAIN_BUFFER_COUNT 3

DXGI_FORMAT SwapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM;

namespace
{
    float s_FrameTime = 0.0f;
    uint64_t s_FrameIndex = 0;
    int64_t s_FrameStartTick = 0;
}

void Display::Initialize(void)
{

}

void Display::Shutdown(void)
{
}

void Display::Resize(uint32_t width, uint32_t height)
{
}

void Display::Present(void)
{
}

uint64_t Graphics::GetFrameCount(void)
{
    return uint64_t();
}

float Graphics::GetFrameTime(void)
{
    return 0.0f;
}

float Graphics::GetFrameRate(void)
{
    return 0.0f;
}
