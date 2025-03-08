#include "footer.h"

#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"

namespace ui::submenus
{
	void footer_submenu::on_init()
	{
		this->add(toggle_option("Enable", {}, &g_footer.m_enabled));
		this->add(number_option("Size", "Change footer size", &g_footer.m_size, 0.05f, 0.001f, 0.005f));
		this->add(color_option("Color", "Change footer bar color", &g_footer.m_color));
	}
}
