#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class vehicle_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern vehicle_submenu g_vehicle_submenu;
}
