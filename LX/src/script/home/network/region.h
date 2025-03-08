#pragma once
#include "gui/types/submenu.hpp"
namespace ui::submenus
{
	class region_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;

	};
	extern region_submenu g_region_submenu;

}
