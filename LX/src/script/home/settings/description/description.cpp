#include "description.h"

#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_option.hpp"

namespace ui::submenus
{
	void description_submenu::on_enter()
	{
		m_font_index = g_description.m_font; 
	}

	void description_submenu::on_init()
	{
		this->add(toggle_option("Enable", {}, &g_description.m_enabled));
		this->add(number_option("Height", "Change description height", &g_description.m_height, 0.05f, 0.001f, 0.005f));
		this->add(number_option("Padding", "Change description Padding", &g_description.m_padding, 30.f, 1.f, 1.f));
		this->add(color_option("Color", "Change description bar color", &g_description.m_color));
		this->add(vector_option("Font", "Change subtitle font", &m_font_index, &font_names, [this]
		{
			g_description.m_font = static_cast<Font>(m_font_index);
		}));
	}
}
