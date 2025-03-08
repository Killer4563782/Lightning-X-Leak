#include "scrollbar.hpp"
#include <gui/types/toggle_option.hpp>

void ui::submenus::scrollbar_submenu::on_init()
{
	this->add(toggle_option("Enable", {}, &g_scrollbar.m_enabled));
}