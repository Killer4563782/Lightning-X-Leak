#pragma once
#include "gui/data.h"

namespace ui
{
	using ShaderResource = ID3D11ShaderResourceView*;

	enum Font : uint8_t
	{
		Arial,
		ArialBold,
		ArialItalic,
		ArialBoldItalic,
		ArialBlack,
		Tahoma,
		FontAwesome,
		Impact,
		RoboticCyborg,
		Ubuntu,
		IconFontData,
	};

	inline std::vector<std::string> font_names {
		"Arial",
		"Arial Bold",
		"Arial Italic",
		"Arial Bold Italic",
		"Arial Black",
		"Tahoma",
		"Font Awesome",
		"Impact",
		"Robotic Cyborg",
		"Ubuntu",
	};

	enum drawing_type 
	{
		Gradient, 
		Static
	};

	inline std::vector<std::string> m_drawing_type {
		"Gradient", 
		"Static"
	}; 

	struct header
	{
		std::string m_text;
		float m_size;
		color m_color;
		Font m_font;
		float m_text_size;
		color m_text_color;
		eJustify m_text_justify;
		bool m_enabled = true;
		void draw();
	};

	struct subtitle
	{
		float m_size;
		float m_padding;
		color m_color;
		Font m_font;
		float m_text_size;
		color m_text_color;
		eJustify m_text_justify;
		float m_option_text_size;
		color m_option_text_color;
		eJustify m_option_text_justify;
		bool m_dont_count_breaks = true;
		bool m_enabled = true;

		color m_divider_color;
		color m_divider_color_left;
		color m_divider_color_right;
		drawing_type m_divider_drawing_type;
		float m_divider_height;

		void draw();
	};

	struct scrollbar
	{
		float m_number_of_options;
		float m_percentage;
		float m_current;
		bool m_enabled = false;

		void draw();
	};

	struct options
	{
		float m_size;
		float m_padding;
		color m_color;
		color m_selected_color;
		Font m_font;
		float m_text_size;
		color m_text_color;
		color m_text_selected_color;
		eJustify m_text_justify;
		float m_scroll_speed;
		Font m_break_font;
		Font m_font_awesome;
		float m_break_text_size;
		color m_break_text_color;
		eJustify m_break_text_justify;
		sprite m_arrow;
		color m_arrow_color;
		color m_arrow_selected_color;
		ImVec2 m_arrow_size;
		sprite m_toggle;
		Font m_toggle_font;
		color m_toggle_color;
		color m_toggle_selected_color;
		ImVec2 m_toggle_size;
		sprite m_globe_sprite;
		sprite m_globe_sprite_bg;
		uint64_t m_max_options_visible_per_submenu;

		static void draw();

		color rect_color(const bool selected) const
		{
			return selected ? m_selected_color : m_color;
		}

		color text_color(const bool selected) const
		{
			return selected ? m_text_selected_color : m_text_color;
		}

		color arrow_color(const bool selected) const
		{
			return selected ? m_arrow_selected_color : m_arrow_color;
		}

		color toggle_color(const bool selected) const
		{
			return selected ? m_toggle_selected_color : m_toggle_color;
		}
	};

	struct footer
	{
		float m_size;
		color m_color;
		sprite m_sprite;
		float m_sprite_size;
		color m_sprite_color;
		bool m_enabled = true;

		color m_divider_color;
		color m_divider_color_left;
		color m_divider_color_right;
		drawing_type m_divider_drawing_type;
		float m_divider_height;

		void draw();

		bool default_sprite() const
		{
			return m_sprite.m_dictionary == "commonmenu" && (m_sprite.m_texture == "shop_arrows_upanddown" || m_sprite.m_texture == "arrowright");
		}
	};

	struct description
	{
		std::string m_text;
		float m_height;
		float m_padding;
		color m_color;
		Font m_font;
		float m_text_size;
		color m_text_color;
		eJustify m_text_justify;
		bool m_enabled = true;

		color m_divider_color;
		float m_divider_height;

		void draw();
	};

	struct watermark 
	{
		bool m_draw;
	};
}
