#include "options.h"

#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/vector_option.hpp"

namespace ui::submenus
{
	void options_submenu::on_enter()
	{
		m_font_index = g_options.m_font;
	}

	void options_submenu::on_init()
	{
		this->add(number_option("Size", "Change option size", &g_options.m_size, 0.05f, 0.001f, 0.005f));
		this->add(number_option("Padding", "Change option Padding", &g_options.m_padding, 30.f, 1.f, 1.f));
		this->add(number_option("Text Size", "Change option text size", &g_options.m_text_size, 0.4f, 0.1f, 0.01f));
		this->add(color_option("Color", "Change option bar color", &g_options.m_color));
		this->add(vector_option("Font", "Change subtitle font", &m_font_index, &font_names, [this]
		{
			g_options.m_font = static_cast<Font>(m_font_index);
		}));
	}
}
