#include "subtitle.h"

#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_option.hpp"

namespace ui::submenus
{
	void subtitle_submenu::on_enter()
	{
		m_font_index = g_subtitle.m_font;
	}

	void subtitle_submenu::on_init()
	{
		this->add(toggle_option("Enable", {}, &g_subtitle.m_enabled));
		this->add(toggle_option("Dont Count Breaks", {}, &g_subtitle.m_dont_count_breaks));
		this->add(number_option("Size", "Change subtitle size", &g_subtitle.m_size, 0.1f, 0.01f, 0.005f));
		this->add(number_option("Padding", "Change subtitle Padding", &g_subtitle.m_padding, 30.f, 1.f, 1.f));
		this->add(color_option("Color", "Change subtitle bar color", &g_subtitle.m_color));
		this->add(vector_option("Font", "Change subtitle font", &m_font_index, &font_names, [this]
		{
			g_subtitle.m_font = static_cast<Font>(m_font_index);
		}));
	}
}
