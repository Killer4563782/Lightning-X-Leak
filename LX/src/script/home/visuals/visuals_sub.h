#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class visuals_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern visuals_submenu g_visuals_submenu;
}
