#pragma once
#include "option.hpp"

namespace ui
{
	class break_option final : public option
	{
	public:
		explicit break_option(std::string name) : option(std::move(name), {}, OptionType::Break)
		{
		}

		void draw(bool selected) override
		{
			drawing::text(
				g_options.m_font,
				m_name,
				{
					g_pos.x + g_width / 2.f,
					g_base + g_options.m_size / 2.f - drawing::get_text_height(g_options.m_font, g_options.m_break_text_size) / 2.f
				},
				g_options.m_break_text_size,
				g_options.m_break_text_color,
				g_options.m_break_text_justify,
				0,
				g_width
			);

			g_base += g_options.m_size;
		}
	};
}
