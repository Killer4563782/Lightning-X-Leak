#include "hooking/hooking.hpp"
#include "imgui/backends/imgui_impl_dx11.h"
#include "renderer/renderer.h"

HRESULT hooks::resizebuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
{
	if (g_running)
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();

		const auto result = g_hooking->m_swapchain_hook->get_original<decltype(&resizebuffers)>(swapchain_resizebuffers_index)(this_, buffer_count, width, height, new_format, swapchain_flags);

		if (SUCCEEDED(result))
		{
			ImGui_ImplDX11_CreateDeviceObjects();
		}

		return result;
	}

	return g_hooking->m_swapchain_hook->get_original<decltype(&resizebuffers)>(swapchain_resizebuffers_index)(this_, buffer_count, width, height, new_format, swapchain_flags);
}
