#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class custom_cars_submenu final : public submenu
	{
		using submenu::submenu;
		void on_enter() override;
	};
	extern custom_cars_submenu g_custom_cars_submenu;
}
