#include "D3DApp.h"

D3DApp::D3DApp(uint32_t width, uint32_t height, std::wstring name)
	: m_Width(width)
	, m_Height(height)
	, m_Title(name)
{

}

D3DApp::~D3DApp()
{
}
