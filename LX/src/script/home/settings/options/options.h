#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class options_submenu final : public submenu
	{
		int m_font_index = g_options.m_font;

		using submenu::submenu;

		void on_enter() override;
		void on_init() override;
	};
}
