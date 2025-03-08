#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class miscellaneous_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern miscellaneous_submenu g_miscellaneous_submenu;
}
