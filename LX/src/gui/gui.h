#pragma once
#include "types.h"

namespace ui
{
	void overlay();											// Draws the overlay
	void draw();											// Draws all components of the menu

	inline float m_select_pos;
	inline color menu_main_color = {51, 108, 184, 255};
	inline color menu_color_white = {255, 255, 255, 255};
	inline color menu_black_color_255 = {0, 0, 0, 255};
	inline color menu_black_color_190 = {0, 0, 0, 190};
	inline color menu_black_color_170 = {0, 0, 0, 170};
	inline color menu_dark_mode = { 20, 20, 20, 255 };
	inline color menu_grey_lines = { 165, 165, 165, 255 };

	inline header g_header({
		.m_text = UPPER_BRAND,
		.m_size = 0.1f,
		.m_color = menu_dark_mode,
		.m_font = RoboticCyborg,
		.m_text_size = 1.2f,
		.m_text_color = menu_color_white,
		.m_text_justify = eJustify::Center,
	});

	inline subtitle g_subtitle({
		.m_size = 0.03f,
		.m_padding = 10.f,
		.m_color = menu_black_color_255,
		.m_font = ArialItalic,
		.m_text_size = 0.27f,
		.m_text_color = menu_color_white,
		.m_text_justify = eJustify::Left,
		.m_option_text_size = 0.27f,
		.m_option_text_color = menu_color_white,
		.m_option_text_justify = eJustify::Right,
		.m_divider_color = menu_grey_lines,
		.m_divider_color_left = menu_main_color,
		.m_divider_color_right = menu_main_color,
		.m_divider_drawing_type = Static,
		.m_divider_height = 0.002f
	});

	inline scrollbar g_scrollbar({
		.m_number_of_options = 0.f,
		.m_percentage = 0.f,
		.m_current = 0.f
	});

	inline options g_options({
		.m_size = 0.03f,
		.m_padding = 10.f,
		.m_color = menu_black_color_190,
		.m_selected_color = menu_color_white,
		.m_font = Arial,
		.m_text_size = 0.28f,
		.m_text_color = menu_color_white,
		.m_text_selected_color = menu_black_color_255,
		.m_text_justify = eJustify::Left,
		.m_scroll_speed = 12.f,
		.m_break_font = RoboticCyborg,
		.m_font_awesome = FontAwesome,
		.m_break_text_size = 0.27f,
		.m_break_text_color = menu_color_white,
		.m_break_text_justify = eJustify::Center,
		.m_arrow = {"commonmenu", "arrowright"},
		.m_arrow_color = menu_color_white,
		.m_arrow_selected_color = menu_black_color_255,
		.m_arrow_size = ImVec2(0.9f, 0.9f),
		.m_toggle = {"commonmenu", "common_medal"},
		.m_toggle_font = IconFontData,
		.m_toggle_color = { 200, 25, 80, 255 },
		.m_toggle_selected_color = { 130, 214, 157, 255 },
		.m_toggle_size = ImVec2(1.3f, 1.3f),
		.m_globe_sprite = {"mprankbadge", "globe"},
		.m_globe_sprite_bg = {"mprankbadge", "globe_bg"},
		.m_max_options_visible_per_submenu = 13
	});

	inline footer g_footer({
		.m_size = 0.03f,
		.m_color = menu_black_color_255,
		.m_sprite = {"commonmenu", "shop_arrows_upanddown"},
		.m_sprite_size = 0.8f,
		.m_sprite_color = menu_color_white,
		.m_divider_color = menu_grey_lines,
		.m_divider_color_left = menu_main_color,
		.m_divider_color_right = menu_main_color,
		.m_divider_drawing_type = Static,
		.m_divider_height = 0.002f
	});

	inline description g_description({
		.m_text = "",
		.m_height = 0.015f,
		.m_padding = 10.f,
		.m_color = menu_black_color_255,
		.m_font = Ubuntu,
		.m_text_size = 0.25f,
		.m_text_color = menu_color_white,
		.m_text_justify = eJustify::Left,
		.m_divider_color = menu_grey_lines,
		.m_divider_height = 0.003f
	});

	inline watermark g_watermark({
		.m_draw = true
	});

	inline float g_width = 0.22f;
	inline ImVec2 g_pos{ 0.03f, 0.05f };
	inline float g_base = 0.f; // Y coord of where menu components should be drawn next
}
