#include "settings.h"
#include "description/description.h"
#include "footer/footer.h"
#include "gui/types/number_option.hpp"
#include "header/header.h"
#include "keybinds/keybinds.h"
#include "options/options.h"
#include "scrollbar/scrollbar.hpp"
#include "subtitle/subtitle.h"
#include "divider/divider.hpp"
#include "gui/types/toggle_option.hpp"

namespace ui::submenus
{
	void settings_submenu::on_tick()
	{
		m_options.clear(); 
		this->add(keybinds_submenu("Keybinds"));
		this->add(header_submenu("Header"));
		this->add(dividers_submenu("Dividers"));
		this->add(subtitle_submenu("Subtitle"));
		this->add(options_submenu("Options"));
		this->add(scrollbar_submenu("Scroll Bar"));
		this->add(footer_submenu("Footer"));
		this->add(description_submenu("Description"));
		this->add(toggle_option("Overlay", "Modify the showingstate of the overlay", &g_watermark.m_draw));
		this->add(number_option("Menu Width", "Modify the width of the menu", &g_width, 0.5f, 0.01f, 0.005f));
		this->add(number_option("Position X", "Change the horizontal position (X-axis) of the menu", &g_pos.x, 1.f - g_width, 0.f, 0.005f));
		this->add(number_option("Position Y", "Change the vertical position (Y-axis) of the menu", &g_pos.y, 1.f, 0.f, 0.005f));
		this->add(number_option("Scrolling Speed", "Adjust the menu scrolling speed", &g_options.m_scroll_speed, 100.f, 1.f, 1.f));
	}

	settings_submenu g_settings_submenu("Settings");
}
