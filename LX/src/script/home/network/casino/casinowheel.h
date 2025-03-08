#pragma once
#include "gui/types/submenu.hpp"
namespace ui::submenus
{
	class casinowheel_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;

	};
	extern casinowheel_submenu g_casinowheel_submenu;

}
