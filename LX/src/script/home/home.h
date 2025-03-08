#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class home_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern home_submenu g_home_submenu;
}
