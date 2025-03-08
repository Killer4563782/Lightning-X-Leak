#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class settings_submenu final : public submenu
	{
		using submenu::submenu;

		void on_tick() override;
	};

	extern settings_submenu g_settings_submenu;
}
