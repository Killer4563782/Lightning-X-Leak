#include "hooking/hooking.hpp"
#include "renderer/renderer.h"

HRESULT hooks::present(IDXGISwapChain* this_, UINT sync_interval, UINT flags)
{
	if (g_running && (flags & static_cast<UINT>(DXGI_PRESENT_TEST)) != static_cast<UINT>(DXGI_PRESENT_TEST))
	{
		g_renderer->on_present();
	}

	return g_hooking->m_swapchain_hook->get_original<decltype(&present)>(swapchain_present_index)(this_, sync_interval, flags);
}