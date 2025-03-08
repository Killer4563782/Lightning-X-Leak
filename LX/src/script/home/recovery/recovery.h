#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class recovery_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	extern recovery_submenu g_recovery_submenu;
}
