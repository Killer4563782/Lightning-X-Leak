#pragma once
#include "gui/types/submenu.hpp"
namespace ui::submenus
{
	class casino_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;

	};

	extern casino_submenu g_casino_submenu;
}
