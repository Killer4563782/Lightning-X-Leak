#include "renderer.h"
#include "fonts/icons.h"
#include "gui/drawing.h"
#include "gui/gui.h"
#include "helpers/folderHelpers.h"
#include "helpers/imageLoaderHelpers.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "memory/pointers.h"
#include "visuals/visuals.h"

// Mostly used for fonts size
// vali: ^ No shit sherlock, what else would it be fucking used for?
inline constexpr auto global_size{ 60.f };

renderer::renderer() : m_dxgi_swapchain(*g_pointers->g_swap_chain)
{
	if (m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_d3d_device)) < 0)
	{
		throw std::runtime_error("Failed to get D3D device.");
	}
	m_d3d_device->GetImmediateContext(&m_d3d_device_context);

	ImGui::SetCurrentContext(ImGui::CreateContext());
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	ImGui_ImplDX11_Init(m_d3d_device, m_d3d_device_context);
	ImGui_ImplWin32_Init(g_pointers->g_hwnd);
	ImGui::StyleColorsClassic();
	style.Colors[ImGuiCol_Button] = { 0.3f, 0.3f, 0.3f, 0.8f };
	style.Colors[ImGuiCol_ButtonActive] = { 0.4f, 0.4f, 0.4f, 0.8f };
	style.Colors[ImGuiCol_ButtonHovered] = { 0.35f, 0.35f, 0.35f, 0.8f };

	style.Colors[ImGuiCol_TitleBgActive] = { 0.1f, 0.1f, 0.1f, 0.7f };
	style.Colors[ImGuiCol_TitleBg] = { 0.1f, 0.1f, 0.1f, 0.6f };
	style.Colors[ImGuiCol_TitleBgCollapsed] = { 0.1f, 0.1f, 0.1f, 0.4f };
	style.Colors[ImGuiCol_WindowBg] = { 0.f, 0.f, 0.f, 0.5f };

	drawing::update();

	auto win_fonts = folder_helpers::get_folder_path(&FOLDERID_Fonts);
	auto lx_fonts = g_lx_path / "Fonts";

	g_font_mgr->add_font_to_atlas(io.Fonts, win_fonts / "arial.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, win_fonts / "arialbd.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, win_fonts / "ariali.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, win_fonts / "arialbi.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, win_fonts / "ariblk.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, win_fonts / "tahoma.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, win_fonts / "impact.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, lx_fonts / "roboticCyborg.ttf", global_size);
	g_font_mgr->add_font_to_atlas(io.Fonts, lx_fonts / "ubuntu.ttf", global_size);

	static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	g_font_mgr->add_font_to_atlas(io.Fonts, lx_fonts / "Special" / "iconFont.ttf", global_size);
	g_font_mgr->append_to_previous_font(io.Fonts, lx_fonts / "Special" / "faSolid900.ttf", icons_ranges, (global_size + 10.f) * 2.f / 3.f);
	g_font_mgr->append_to_previous_font(io.Fonts, lx_fonts / "Special" / "fontAwesome.ttf", icons_ranges, (global_size + 10.f) * 2.f / 3.f);

	m_arial =           g_font_mgr->get_font("arial.ttf").value();
	m_arialBold =       g_font_mgr->get_font("arialbd.ttf").value();
	m_arialItalic =     g_font_mgr->get_font("ariali.ttf").value();
	m_arialBoldItalic = g_font_mgr->get_font("arialbi.ttf").value();
	m_arialBlack =      g_font_mgr->get_font("ariblk.ttf").value();
	m_tahoma =          g_font_mgr->get_font("tahoma.ttf").value();
	m_impact =          g_font_mgr->get_font("impact.ttf").value();
	m_fontAwesome =     g_font_mgr->get_font("fontAwesome.ttf").value();
	m_iconFont =        g_font_mgr->get_font("iconFont.ttf").value();
	m_roboticCyborg =   g_font_mgr->get_font("roboticCyborg.ttf").value();
	m_ubuntu =          g_font_mgr->get_font("ubuntu.ttf").value();

	style.AntiAliasedFill = true; // Anti-Aliasing - Fill
	style.AntiAliasedLines = true;// Anti-Aliasing - Lines
	style.AntiAliasedLinesUseTex = true; // Anti-Aliasing - texture for lines
}

renderer::~renderer()
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

	// create copy of m_wndproc_callbacks
	for (auto wndproc_callbacks = m_wndproc_callbacks; const auto& callback : wndproc_callbacks | std::views::values)
		callback(hwnd, msg, wparam, lparam);
}

void renderer::on_present() const
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	g_font_mgr->update(io.Fonts);
	ImGui::NewFrame();
	{
		// create copy of m_render_queue
		for (auto render_queue = m_render_queue; const auto& callback : render_queue | std::views::values)
			callback();

		drawing::update();							// Update Drawing
		visuals::draw_all();						// Draw Visuals
		overlay();									// Draw Overlay
		image_loader::header_handler();				// Draw Header
		draw();										// Draw Menu
		ImGui::render_notifications();				// Draw Notifications
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}