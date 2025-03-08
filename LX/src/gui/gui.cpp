#include "gui.h"

#include "drawing.h"
#include "interface.h"
#include "helpers/imageLoaderHelpers.h"
#include "helpers/playerInfo.h"
#include "types/option.hpp"
#include "types/submenu.hpp"

namespace ui
{
	void overlay()
	{
		if (g_watermark.m_draw)
		{
			std::string time;
			time.resize(80);
			const auto time_c{ std::time(nullptr) };
			std::strftime(time.data(), time.size(), "%c", std::localtime(&time_c));

			color left_color{ 51, 108, 184, 255 };
			color right_color{ 70, 200, 255, 255 };
			color text_color{ 255, 255, 255, 255 };
			color top_rect_color{ 31, 31, 31, 255 };
			ImVec2 pos{ 0.004f, 0.01f };
			drawing::rectangle({ pos.x, pos.y }, { 0.205f, 0.03f }, top_rect_color);
			drawing::gradient_rectangle({ pos.x, pos.y + 0.03f }, { 0.205f, 0.005f }, left_color, right_color, right_color, left_color);
			drawing::text(Ubuntu, std::format(BRAND " | {} FPS | {}", std::round(drawing::g_io->Framerate), time), { pos.x + 0.004f, pos.y + 0.005f }, 0.32f, text_color, eJustify::Left, 0.f, g_width);
		}
	}

	void header::draw()
	{
		if (!m_enabled) return;

		if (!image_loader::has_header_loaded)
		{
			drawing::rectangle({ g_pos.x, g_base }, { g_width, m_size }, m_color, false);
			drawing::text(m_font, m_text,
			{
				g_pos.x + g_width / 2.f,
				g_base + m_size / 2.f - drawing::get_text_size(m_font, m_text_size, m_text).y / 2.f
			},
			m_text_size, m_text_color, m_text_justify, 2.f, g_width); 
			
		}
		g_base += m_size;
	}

	void subtitle::draw()
	{
		if (!m_enabled)
			return;

		drawing::rectangle({g_pos.x, g_base}, {g_width, m_size}, m_color);

		std::string path = g_current_menu->m_name;
		const submenu* parent = g_current_menu->m_parent;
		while (parent)
		{
			path.insert(0, parent->m_name + " / ");
			parent = parent->m_parent;
		}

		drawing::text(
			g_subtitle.m_font,
			path,
			{g_pos.x, g_base + m_size / 2.f - drawing::get_text_size(g_subtitle.m_font, m_text_size, path).y / 2.f},
			m_text_size,
			m_text_color,
			m_text_justify,
			m_padding,
			g_width
		);

		auto num_options_calc = g_current_menu->m_options.size();
		auto selected_option_calc = g_current_menu->m_current + 1;
		if (m_dont_count_breaks)
		{
			for (uint64_t i = 0; i < g_current_menu->m_options.size(); ++i)
			{
				if (g_current_menu->get_option(i)->m_type == OptionType::Break)
				{
					if (i <= g_current_menu->m_current)
						selected_option_calc--;

					num_options_calc--;
				}
			}
		}
		const auto option_text = std::format("{} / {}", g_current_menu->m_options.empty() ? 0ULL : selected_option_calc, num_options_calc);
		
		drawing::text(
			g_subtitle.m_font,
			to_upper(option_text),
			{
				g_pos.x + g_width,
				g_base + m_size / 2.f - drawing::get_text_size(g_subtitle.m_font, m_option_text_size, option_text).y /
				2.f
			},
			m_option_text_size,
			m_option_text_color,
			m_option_text_justify,
			-m_padding,
			g_width
		);

		//Line below subtitle
		drawing::rectangle({ g_pos.x, g_base + m_size - 0.002f }, { g_width, 0.002f }, m_divider_color);

		g_base += m_size;
	}

	void scrollbar::draw()
	{
		if (!m_enabled)
			return;

		const auto& options = g_current_menu->m_options;
		const auto m_current = g_current_menu->m_current;

		const float max_visible_options = static_cast<float>(g_options.m_max_options_visible_per_submenu);
		const float num_options = static_cast<float>(options.size());
		const float option_size = g_options.m_size;

		ImVec2 background_position = {g_pos.x - 0.015f, g_base};
		ImVec2 background_size = {0.0085f, option_size * std::min(num_options, max_visible_options)};

		static float scroller_y = background_position.y;
		float target_scroller_y = background_position.y + background_size.y / num_options * m_current;
		scroller_y += (target_scroller_y - scroller_y) * g_options.m_scroll_speed / 100;

		// Prevent scroller from going below or above the background size
		scroller_y = std::max(background_position.y, std::min(scroller_y, background_position.y + background_size.y - option_size));

		ImVec2 scroller_position = {background_position.x + 0.0007f, scroller_y};
		ImVec2 scroller_size = {0.0072f, option_size};

		drawing::rectangle(background_position, background_size, g_options.m_color, false, 2.f, ImDrawFlags_RoundCornersAll);
		drawing::rectangle(scroller_position, scroller_size, menu_color_white, false, 2.f, ImDrawFlags_RoundCornersAll);

		// Top rectangle at the height of the subtitle
		drawing::rectangle({ background_position.x , g_base - g_options.m_size }, { background_size.x, g_subtitle.m_size }, g_options.m_color, false, 2.f, ImDrawFlags_RoundCornersTop);

		// Line to separate the subtitle from the scrollbar
		drawing::rectangle({ background_position.x, g_base - 0.002f}, { background_size.x, 0.002f }, menu_grey_lines);

		// Inside the rectangle center add this text
		drawing::text(g_options.m_toggle_font, "d", { background_position.x + background_size.x + 0.0015f, g_base - g_options.m_size + 0.0085f }, 0.23f, menu_color_white, eJustify::Center, -g_options.m_padding, g_width);

		// Bottom rectangle at the height of the footer
		drawing::rectangle({ background_position.x , g_base + background_size.y }, { background_size.x, g_footer.m_size }, g_options.m_color, false, 2.f, ImDrawFlags_RoundCornersBottom);

		// Line to separate the scrollbar from the footer
		drawing::rectangle({ background_position.x, g_base + background_size.y }, { background_size.x, 0.002f }, menu_grey_lines);

		// Inside the rectangle center add this text
		drawing::text(g_options.m_toggle_font, "c", { background_position.x + background_size.x + 0.0015f, g_base + background_size.y + 0.0085f }, 0.23f, menu_color_white, eJustify::Center, -g_options.m_padding, g_width);
	}

	void options::draw()
	{
		if (g_current_menu->m_options.empty())
			return;

		if (g_current_menu->m_current >= g_current_menu->m_options.size())
		{
			g_current_menu->m_current = g_current_menu->m_options.size() - 1;
			g_current_menu->m_scroll_position = g_current_menu->m_current >= g_options.m_max_options_visible_per_submenu ? g_current_menu->m_current - g_options.m_max_options_visible_per_submenu + 1 : 0;
		}

		if (g_current_menu->get_option()->m_type == OptionType::Break && std::ranges::any_of(g_current_menu->m_options, [](const option* opt) { return opt->m_type != OptionType::Break; }))
			g_current_menu->action(ActionType::Down);

		// Calculate the size of the menu based on options and visibility.
		const auto size = g_options.m_size * std::max(1.f, std::min(static_cast<float>(g_current_menu->m_options.size()), static_cast<float>(g_options.m_max_options_visible_per_submenu)));

		drawing::rectangle({g_pos.x, g_base}, {g_width, size}, g_options.m_color);

		if (const auto target_pos = g_options.m_size * std::min(static_cast<float>(g_current_menu->m_current - g_current_menu->m_scroll_position), static_cast<float>(g_options.m_max_options_visible_per_submenu - 1)); abs(m_select_pos - target_pos) > 0.0001f)
		{
			m_select_pos += (target_pos - m_select_pos) * g_options.m_scroll_speed / 100;
		}

		drawing::rectangle({g_pos.x, g_base + m_select_pos}, {g_width, g_options.m_size}, g_options.m_selected_color);

		for (auto i = g_current_menu->m_scroll_position; i < g_current_menu->m_options.size() && i < g_current_menu->m_scroll_position + g_options.m_max_options_visible_per_submenu; ++i)
		{
			g_current_menu->get_option(i)->draw(g_current_menu->m_current == i);
		}
	}

	void footer::draw()
	{
		if (!m_enabled)
			return;

		auto size = m_sprite_size;
		if (g_current_menu && default_sprite())
		{
			if (g_current_menu->m_current == 0 || g_current_menu->m_current >= g_current_menu->m_options.size() - 1)
			{
				m_sprite.m_texture = "arrowright";
				m_sprite.m_rotation = g_current_menu->m_current == 0 ? 360.f : -180.f;
			}
			else
			{
				m_sprite.m_texture = "shop_arrows_upanddown";
				m_sprite.m_rotation = 0.f;
				size *= 0.6f;
			}
		}

		drawing::rectangle({g_pos.x, g_base}, {g_width, m_size}, m_color, false);
		drawing::image(m_sprite, {g_pos.x + g_width / 2, g_base + m_size / 2}, {size, size}, m_sprite_color);
		drawing::text(
			g_options.m_font,
			"V" MENU_VERSION,
			{
				g_pos.x,
				g_base + m_size / 2 - drawing::get_text_size(g_options.m_font, g_options.m_text_size, "V" MENU_VERSION).
				y /
				2.f
			},
			g_options.m_text_size,
			menu_color_white,
			eJustify::Left,
			g_options.m_padding,
			g_width
		);

		// Line above footer
		drawing::rectangle({ g_pos.x, g_base }, { g_width, 0.002f }, m_divider_color);

#ifdef _DEBUG
		drawing::text(
			g_options.m_font,
			"DEV",
			{
				g_pos.x + g_width,
				g_base + m_size / 2 - drawing::get_text_size(g_options.m_font, g_options.m_text_size, "DEV").y / 2.f
			},
			g_options.m_text_size,
			menu_color_white,
			eJustify::Right,
			-g_options.m_padding,
			g_width
		);
#endif

		g_base += m_size;
	}

	void description::draw()
	{
		if (!m_enabled)
			return;

		if (const auto option = g_current_menu->get_option(); !option || option->m_description.empty())
			return;
		else m_text = option->m_description;

		g_base += m_height;
		const auto wrap = g_width - 0.015f;
		const auto line_count = drawing::get_line_count(m_text, m_font, m_text_size, wrap);
		auto size = (drawing::get_text_height(m_font, m_text_size, wrap) + 0.005f) * static_cast<float>(line_count) + (line_count <= 1 ? 0.01f : 0.005f);
		drawing::rectangle({g_pos.x, g_base}, {g_width, size}, m_color, false);
		drawing::rectangle({g_pos.x, g_base}, {g_width, 0.003f}, menu_grey_lines); // Line above description.
		drawing::text(
			m_font,
			m_text,
			{
				g_pos.x,
				g_base + size / 8 + drawing::get_text_size(m_font, m_text_size, m_text).y / static_cast<float>(
					line_count) / 2 - 0.001f
			},
			m_text_size,
			m_text_color,
			m_text_justify,
			m_padding,
			wrap
		);

		g_base += size;
	}

	void draw()
	{
		drawing::update();
		if (g_current_menu)
		{
			handlers::key::actions();
			if (!g_current_menu->m_initialized)
			{
				g_current_menu->on_init();
				g_current_menu->m_initialized = true;
			}
			g_current_menu->on_tick();

			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
			if (ImGui::Begin("##UI", nullptr, 935) && g_open)
			{
				g_base = g_pos.y;
				g_header.draw();
				g_subtitle.draw();
				g_scrollbar.draw();
				options::draw();
				g_footer.draw();
				g_description.draw();
			}
			ImGui::End();
		}
	}
}
