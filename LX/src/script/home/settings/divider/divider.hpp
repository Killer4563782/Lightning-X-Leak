#pragma once 
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class dividers_submenu final : public submenu {

		int m_subtitle_divider_index = g_subtitle.m_divider_drawing_type;
		int m_footer_divider_index = g_footer.m_divider_drawing_type;

		using submenu::submenu;
		void on_tick() override;
		void on_enter() override;
	};
}