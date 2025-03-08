#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class network_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern network_submenu g_network_submenu;
}
