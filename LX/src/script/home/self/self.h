#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class self_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern self_submenu g_self_submenu;
}
