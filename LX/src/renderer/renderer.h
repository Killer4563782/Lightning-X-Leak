#pragma once
#include "font_manager/font_manager.hpp"

using dx_callback = std::function<void()>;
using wndproc_callback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;

class renderer final
{
public:
    renderer();
    ~renderer();

    void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void on_present() const;

    // Fonts
    font* m_arial;
    font* m_arialBold;
    font* m_arialItalic;
    font* m_arialBoldItalic;
    font* m_arialBlack;
    font* m_tahoma;
    font* m_impact;

    // Custom Fonts
    font* m_fontAwesome;
    font* m_iconFont;
    font* m_roboticCyborg;
    font* m_ubuntu;

    std::map<std::string, dx_callback> m_render_queue;
    IDXGISwapChain* m_dxgi_swapchain;
    ID3D11Device* m_d3d_device;
    ID3D11DeviceContext* m_d3d_device_context;

    std::map<uint32_t, dx_callback> m_dx_callbacks;
    std::map<uint32_t, wndproc_callback> m_wndproc_callbacks;
};

inline std::unique_ptr<renderer> g_renderer{};