#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class weapons_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern weapons_submenu g_weapons_submenu;
}
